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

Thought of steps, adapted from the link mentioned above (can be changed if some are found to be wrong)
1. Initialize MPI environment. DONE
2. Divide vertices among MPI processes. DONE
3. Initialize local data structures (subsets, cheapest edges). DONE
4. while (multiple connected components exist - more than one component altogether):
    Each process computes the cheapest edges for its vertices locally. DONE
    Exchange information about cheapest edges with other processes. (probably done via broadcast?) - look at friday.
    Update local data structures based on received information. DONE...?
    Check for termination condition (if there is one component left)
9. Gather MST edges from all processes and construct the final MST.
10. Output the final MST.
11. Finalize MPI environment.

*/

Graph createGraph(int numVertices)
{
    Graph res(numVertices);
    // todo: add edges by calling res.addEdge(x, y, weight) - maybe script?
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

void newSet(Set* set, const int elements) {
	set->elements = elements;
	set->canonicalElements = (int*) malloc(elements * sizeof(int)); // maintain parent 
	memset(set->canonicalElements, UNSET_ELEMENT, elements * sizeof(int));
	set->rank = (int*) calloc(elements, sizeof(int)); //  maintain rank
}

// Function to find the subset of an element using path compression
int find(Subset subsets[], int i)
{
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

int findSet(const Set* set, const int vertex) {
	if (set->canonicalElements[vertex] == UNSET_ELEMENT) {
		return vertex;
	} 
	else {
		set->canonicalElements[vertex] = findSet(set,set->canonicalElements[vertex]);
		return set->canonicalElements[vertex];
	}
}

// Function to perform union of two subsets using union by rank
void unionSets(Subset subsets[], int x, int y)
{
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);
    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;
    else
    {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

// // copy an edge
// void copyEdge(int* to, int* from) {
// 	memcpy(to, from, 3 * sizeof(int));
// }

// copy an edge
void copyEdge(Edge* copyDestination, Edge* copySource) {
    //not sure if it should be 3 * sizeof(int)...
	memcpy(copyDestination, copySource, 3 * sizeof(int));
}

void distributeAndScatterEdges(std::vector<Edge> &edges, std::vector<Edge> &localEdges, int world_rank, int world_size)
{
    int localEdgeCount = edges.size() / world_size;
    localEdges.resize(localEdgeCount);
    MPI_Scatter(edges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, localEdges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    // send remaining edges to the last process
    if (world_rank == 0)
    {
        std::vector<Edge> remaining;
        int remainingEdges = edges.size() % world_size;
        remaining.insert(remaining.end(), edges.end() - remaining, edges.end());

        MPI_Send(remaining.data(), remaining.size() * sizeof(Edge), MPI_BYTE, world_size - 1, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == world_size - 1)
    {
        MPI_Status status;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_BYTE, &count);
        std::vector<Edge> remaining(count / sizeof(Edge));
        MPI_Recv(remaining.data(), count, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        localEdges.insert(localEdges.end(), remaining.begin(), remaining.end());
    }
}

// assume a parallel version, then just use mpi for communication
void boruvkasAlgorithmMPI(std::vector<Edge> edges, int numVertices, int world_rank, int world_size)
{
    std::vector<Edge> mstEdgesSelected;
    std::vector<Edge> localEdges;

    distributeAndScatterEdges(edges, localEdges, world_rank, world_size);
    MPI_Barrier(MPI_COMM_WORLD);

    // NOTE: i think code below is too serial in nature, need to parallelize it to somehow make it work with MPI.
    // ======================================================

    // Subset *subsets = new Subset[numVertices];
    // int *cheapest = new int[numVertices];

    // // Initialize subsets and cheapest arrays
    // for (int v = 0; v < numVertices; v++)
    // {
    //     subsets[v].parent = v;
    //     subsets[v].rank = 0;
    //     cheapest[v] = -1;
    // }
    // create needed data structures
	Set* set = &(Set ) { .elements = 0, .canonicalElements = NULL, .rank =NULL };
	newSet(set, numVertices);

    // MPI_Barrier(MPI_COMM_WORLD);

    // in the beginning, each process is aware that there are |V| components.
    // I think this needs to be global...
    int numSubtreesRemaining = numVertices;
    int localMstWeight = 0;

    int edgesMST = 0;
	//int* closestEdge = (int*) malloc(vertices * 3 * sizeof(int));
    Edge *closestEdge = new Edge[numVertices];
	//int* closestEdgeRecieved;
    Edge *closestEdgeRecieved;
	if (parallel) {
		//closestEdgeRecieved = (int*) malloc(vertices * 3 * sizeof(int));
        closestEdgeRecieved = new Edge[numVertices]
	}

	for (int i = 1; i < vertices && edgesMST < vertices - 1; i *= 2) {
		// reset all closestEdge
		for (int j = 0; j < vertices; j++) {
			closestEdge[j].setWeight(INT_MAX);
		}

		// find closestEdge
        //not sure if that is the right substitution?
		//for (int j = 0; j < edgesPart; j++) {
        for (int j = 0; j < localEdges.size(); j++) {
			//int* currentEdge = &edgeListPart[j * 3];
            Edge *currentEdge = &localEdges[j];
			//int canonicalElements[2] = { findSet(set, currentEdge[0]), findSet(set, currentEdge[1]) };

            //THIS IS AN ARRAY THATS NOT PART OF THE SET
            int localCanonicalElements[2] = { findSet(set, currentEdge.getFirstVertex()), findSet(set, currentEdge.getSecondVertex()) }

            // WTF IS A CANONICAL EDGE ?!?!?!?!??!?!?!?!?
			// eventually update closestEdge
			if (localCanonicalElements[0] != localCanonicalElements[1]) {
				for (int k = 0; k < 2; k++) {
					//bool closestEdgeNotSet = closestEdge[canonicalElements[k]* 3 + 2] == INT_MAX;
                    bool closestEdgeNotSet = closestEdgeNotSet[localCanonicalElements[k]].getWeight() == INT_MAX;
					//bool weightSmaller = currentEdge[2] < closestEdge[canonicalElements[k] * 3	+ 2];
                    bool weightSmaller = currentEdge.getWeight() < closestEdge[localCanonicalElements[k]].getWeight();
					if (closestEdgeNotSet || weightSmaller) {
                        // i dont think this is the rigth mapping
						//copyEdge(&closestEdge[canonicalElements[k] * 3],currentEdge);
                        copyEdge(&closestEdge[canonicalElements[k]], currentEdge);
					}
				}
			}
		}

        //no real clue whats going on here
		if (parallel) {
			int from;
			int to;
			for (int step = 1; step < size; step *= 2) {
				if (rank % (2 * step) == 0) {
					from = rank + step;
					if (from < size) {
						MPI_Recv(closestEdgeRecieved, vertices * 3,	MPI_INT, from, 0, MPI_COMM_WORLD, &status);

						// combine all closestEdge parts
						for (int i = 0; i < vertices; i++) {
							int currentVertex = i * 3;
							if (closestEdgeRecieved[currentVertex + 2]< closestEdge[currentVertex + 2]) {
								copyEdge(&closestEdge[currentVertex],&closestEdgeRecieved[currentVertex]);
							}
						}
					}
				} 
				else if (rank % step == 0) {
					to = rank - step;
					MPI_Send(closestEdge, vertices * 3, MPI_INT, to,0,MPI_COMM_WORLD);
				}
			}
			// publish all closestEdge parts
			MPI_Bcast(closestEdge, vertices * 3, MPI_INT, 0,MPI_COMM_WORLD);
		}

		// add new edges to MST
		for (int j = 0; j < vertices; j++) {
			if (closestEdge[j * 3 + 2] != INT_MAX) {
                //
				int from = closestEdge[j * 3];
				int to = closestEdge[j * 3 + 1];

				// prevent adding the same edge twice
				//if (findSet(set, from) != findSet(set, to)) {
                if (findSet(set, closestEdge[j].getFirstVertex()) != findSet(set, closestEdge[j].getSecondVertex())) {
					//if (rank == 0) {
                    if (world_rank == 0) {
						copyEdge(&mst->edgeList[edgesMST * 3],&closestEdge[j * 3]);
					}
					edgesMST++;
					//unionSet(set, from, to);
                    unionSet(set, closestEdge[j].getFirstVertex(), closestEdge.getSecondVertex());
				}
			}
		}
	}

    // since each process will have its own mst weight after unioning everything, will we have to gather the weights? maybe not?
    // maybe send to root?

    // if we have the root process, then we print the MST accordingly.
    if (world_rank == 0)
    {
        // std::cout << "Total Minimum Spanning Tree Weight: " << totalMSTWeight << std::endl;
    }
}

int world_rank = 0;
int world_size = 0;

int main()
{
    cxxopts::Options options(
        "spanning_tree_mpi",
        "Calculate minimum spanning using distributed (mpi) execution");

    options.add_options(
        "",
        {
            {"inputFile", "Input graph file path",
             cxxopts::value<std::string>()->default_value(
                 "./graph_inputs/random_10.txt")},
        });

    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();

    // Initialize the MPI environment
    int pid, np;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (np < 1)
    {
        std::cerr << "Number of processes must be greater than 0" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    Graph g;
    g.readGraphFromFile(input_file_path);

    if (g.getNumVertices() < np)
    {
        std::cerr << "Number of vertices must be greater than or equal to the number of processes" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    boruvkasAlgorithmMPI(g.getGraphEdges(), g.getNumVertices(), pid, np);
    return 0;
}