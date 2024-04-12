#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <algorithm>
#include <mpi.h>

#include "core/graph.h"
#include "core/edge.h"
#include "core/get_time.h"
#include "core/subset.h"

// Code is based off the serial boruvka's code here: https://www.tutorialspoint.com/boruvka-s-algorithm-for-minimum-spanning-tree-in-cplusplus
// and then adapted below to be compatible with MPI.

/*
Implementing Boruvka's Algorithm in Parallel using MPI.
- initialize the world size to however many vertices there are.
    - in boruvka's algorithm, we have |V| "components". So therefore, the MPI world size
    must be the number of vertices to begin the algorithm. (implement an error check)
- each process(component) then will find the smallest edge which connects it to another component.
- We then merge the two components, and then repeat the above steps until we only have one complete component.
*/

Graph createGraph(int numVertices) {
    Graph res(numVertices);
    //todo: add edges by calling res.addEdge(x, y, weight) - maybe script?
    Edge e1(0, 2, 11);
    Edge e2(1, 6, 3);
    Edge e3(1, 4, 5);
    Edge e4(2, 9, 10);
    Edge e5(5, 2, 1);
    Edge e6(5, 4, 6);
    Edge e7(7, 1, 15);
    Edge e8(9, 5, 8);
    Edge e9(4, 7, 4);
    Edge e10(8, 5, 4);
    Edge e11(0, 5, 8);
    Edge e12(6, 5, 16);
    Edge e13(7, 2, 13);
    Edge e14(1, 5, 11);
    Edge e15(3, 8, 1);
    Edge e16(3, 5, 3);

    res.addEdge(e1);
    res.addEdge(e2);
    res.addEdge(e3);
    res.addEdge(e4);
    res.addEdge(e5);
    res.addEdge(e6);
    res.addEdge(e7);
    res.addEdge(e8);
    res.addEdge(e9);
    res.addEdge(e10);
    res.addEdge(e11);
    res.addEdge(e12);
    res.addEdge(e13);
    res.addEdge(e14);
    res.addEdge(e15);
    res.addEdge(e16);
    return res;
}

// Function to find the subset of an element using path compression
int find(Subset subsets[], int i) {
   if (subsets[i].parent != i)
      subsets[i].parent = find(subsets, subsets[i].parent);
   return subsets[i].parent;
}

// Function to perform union of two subsets using union by rank
void unionSets(Subset subsets[], int x, int y) {
   int xroot = find(subsets, x);
   int yroot = find(subsets, y);
   if (subsets[xroot].rank < subsets[yroot].rank)
      subsets[xroot].parent = yroot;
   else if (subsets[xroot].rank > subsets[yroot].rank)
      subsets[yroot].parent = xroot;
   else {
      subsets[yroot].parent = xroot;
      subsets[xroot].rank++;
   }
}

void boruvkasAlgorithmMPI(std::vector<Edge> edges, int numVertices, int world_rank, int world_size) {
    std::vector<Edge> localEdges;

    //assign edges to each process; root one gets the remainder.
    int localEdgeCount = edges.size() / world_size; // num vertices also == world_size
    std::cout << "process " << world_rank << " has " << localEdgeCount << " edges \n";
    localEdges.resize(localEdgeCount);
    //todo: deal with remainder should we have an odd number?
    MPI_Scatter(edges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, localEdges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    for (auto e : localEdges) {
        std::cout << "world rank: " << world_rank << " with edge: " << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    Subset* subsets = new Subset[numVertices];
    int* cheapest = new int[numVertices];

    // Initialize subsets and cheapest arrays
    for (int v = 0; v < numVertices; v++) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
        cheapest[v] = -1;
    }

    // in the beginning, each process is aware that there are |V| components.
    int numSubtreesRemaining = numVertices;
    int mstWeight = 0;

    while (numSubtreesRemaining > 1) {
        // iterate through all the edges in the given subset 'localEdges' and 
        // find the cheapest weighted edge for each subset.
        for (int i = 0; i < localEdges.size(); i++) {
            int set1 = find(subsets, localEdges[i].getFirstVertex());
            int set2 = find(subsets, localEdges[i].getSecondVertex());

            if (set1 != set2) {
                if (cheapest[set1] == -1 || edges[cheapest[set1]].getWeight() > localEdges[i].getWeight())
                    cheapest[set1] = i;
                if (cheapest[set2] == -1 || edges[cheapest[set2]].getWeight() > localEdges[i].getWeight())
                    cheapest[set2] = i;
            }
        }

        // synchronization phase?
        int* globalCheapest = new int[numVertices * world_size];
        MPI_Allgather(cheapest, numVertices, MPI_INT, globalCheapest, numVertices, MPI_INT, MPI_COMM_WORLD);

        // iterate through all the subsets and if a minimum weighted edge exists for a subset, 
        // we add it to the selectedEdges vector, update the MSTWeight, perform the union of the subsets, and decrement the numTrees.
        for (int v = 0; v < numVertices; v++) {
            int minEdge = -1;
            for (int p = 0; p < world_size; p++) {
                int curEdge = globalCheapest[p * numVertices + v];
                if (curEdge != -1 && (minEdge == -1 || localEdges[curEdge].getWeight() < localEdges[minEdge].getWeight())) {
                    minEdge = curEdge;
                }
            }

            if (minEdge != -1) {
                int set1 = find(subsets, localEdges[minEdge].getFirstVertex());
                int set2 = find(subsets, localEdges[minEdge].getSecondVertex());

                if (set1 != set2) {
                    mstWeight += localEdges[minEdge].getWeight();
                    unionSets(subsets, set1, set2);
                    numSubtreesRemaining--;
                }

                cheapest[v] = -1;
            }
        }

        delete[] globalCheapest;
    }
    
    //since each process will have its own mst weight after unioning everything, we must gather those numbers
    //and use them in the root process.
    int* allMSTWeights = new int[world_size];
    MPI_Gather(&mstWeight, 1, MPI_INT, allMSTWeights, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //if we have the root process, then we print the MST accordingly.
    if (world_rank == 0) {
        int totalMSTWeight = 0;
        for (int i = 0; i < world_size; i++) {
            totalMSTWeight += allMSTWeights[i];
        }
        std::cout << "Total Minimum Spanning Tree Weight: " << totalMSTWeight << std::endl;
    }
}

int world_rank = 0;
int world_size = 0;

int main() {
    // Initialize the MPI environment
    Graph g = createGraph(10);
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    boruvkasAlgorithmMPI(g.getGraphEdges(), world_size, world_rank, world_size);
    return 0;
}