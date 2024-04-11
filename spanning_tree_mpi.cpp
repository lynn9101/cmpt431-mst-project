#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <algorithm>

#include "core/graph.h"
#include "core/edge.h"
#include "core/get_time.h"
#include "core/subsets.h"

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

void boruvkasAlgorithmMPI(std::vector<Edge> &edges, int numVertices, int world_rank) {
    std::vector<Edge> localEdges;

    //assign edges to each process; root one gets the remainder.
    int localEdgeCount = edges.size() / numVertices; // num vertices also == world_size
    localEdges.resize(localEdgeCount);
    //todo: deal with remainder should we have an odd number

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

        // synchronization phase?

        // iterate through all the subsets and if a minimum weighted edge exists for a subset, 
        // we add it to the selectedEdges vector, update the MSTWeight, perform the union of the subsets, and decrement the numTrees.
    }
    
    //since each process will have its own mst weight after unioning everything, we must gather those numbers
    //and use them in the root process.

    //if we have the root process, then we print the MST accordingly.
    if (world_rank == 0) {
        
    }
}

int world_rank = 0;
int world_size = 0;

int main() {
    // Initialize the MPI environment
    Graph g = createGraph(world_size);
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    return 0;
}