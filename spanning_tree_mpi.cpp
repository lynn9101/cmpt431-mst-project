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

<<<<<<< Updated upstream
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
=======
// // Function to find the subset of an element using path compression
// int find(Subset subsets[], int i)
// {
//     if (subsets[i].parent != i)
//         subsets[i].parent = find(subsets, subsets[i].parent);
//     return subsets[i].parent;
// }

//return the canonical element of a vertex with path compression
int findSet(const Subset* set, const int vertex) {
	if (set->parent[vertex] == UNSET_ELEMENT) {
		return vertex;
	} 
	else {
		set->parent[vertex] = findSet(set, set->parent[vertex]);
		return set->parent[vertex];
	}
}

void unionSet(Subset* set, const int parent1, const int parent2) {
	int root1 = findSet(set, parent1);
	int root2 = findSet(set, parent2);

	if (root1 == root2) {
		return;
	} 
	// Attach smaller rank tree under root of high
	else if (set->rank[root1] < set->rank[root2]) {
		set->parent[root1] = root2;
	} 
	else if (set->rank[root1] > set->rank[root2]) {
		set->parent[root2] = root1;
	} 
	// If ranks are same, then make one as root and 
    // increment its rank by one
	else {
		set->parent[root1] = root2;
		set->rank[root2] = set->rank[root1] + 1;
	}
}

void scatterEdgeList(int* edgeList, int* edgeListPart, const int elements,int* elementsPart) {
	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Scatter(edgeList, *elementsPart * 3, MPI_INT, edgeListPart,	*elementsPart * 3, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == size - 1 && elements % *elementsPart != 0) {
		// number of elements and processes isn't divisible without remainder
		*elementsPart = elements % *elementsPart;
	}

	if (elements / 2 + 1 < size && elements != size) {
		if (rank == 0) {
			fprintf(stderr, "Unsupported size/process combination, exiting!\n");
		}
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
}

void printWeightedGraph(const Graph* graph) {
	printf("------------------------------------------------\n");
	// for (int i = 0; i < graph->getGraphEdges().size(); i++) {
	// 	for (int j = 0; j < 3; j++) {
	// 		//printf("%d\t", graph->edgeList[i * 3 + j]);
            
	// 	}
	// 	//printf("\n");
    //     std::cout << std::endl;
	// }
    for (auto e : graph->getGraphEdges())
    {
        std::cout << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }

	printf("------------------------------------------------\n");
>>>>>>> Stashed changes
}

void boruvkasAlgorithmMPI(std::vector<Edge> edges, int numVertices, int world_rank, int world_size) {
    std::vector<Edge> mstEdgesSelected;
    std::vector<Edge> localEdges;
    //int numEdges = edges.size();

<<<<<<< Updated upstream
    //assign edges to each process; root one gets the remainder.
    int localEdgeCount = edges.size() / world_size; // num vertices also == world_size
    std::cout << "process " << world_rank << " has " << localEdgeCount << " edges \n";
    localEdges.resize(localEdgeCount);
    //todo: deal with remainder should we have an odd number?
    MPI_Scatter(edges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, localEdges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    for (auto e : localEdges) {
        std::cout << "world rank: " << world_rank << " with edge: " << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }

    //MPI_Barrier(MPI_COMM_WORLD);

    Subset* subsets = new Subset[numVertices];
    int* cheapest = new int[numVertices];

    // Initialize subsets and cheapest arrays
    for (int v = 0; v < numVertices; v++) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
        cheapest[v] = -1;
    }

    //MPI_Barrier(MPI_COMM_WORLD);
=======
    // assign edges to each process; root one gets the remainder.
    // int localEdgeCount = numEdges / world_size;
    // localEdges.resize(localEdgeCount);

    // int remainder = numEdges % world_size;
    // int begin = world_rank * localEdgeCount;
    // int end = begin + localEdgeCount;

    // // std::cout << "process " << world_rank << " has " << localEdgeCount << " edges \n";
    // localEdges = std::vector<Edge>(edges.begin() + begin, edges.begin() + end);
    
    // // root process takes the remainder
    // if (world_rank == 0)
    // {
    //     localEdges.insert(localEdges.end(), edges.end() - remainder, edges.end());
    // }
    bool isParallel = size != 1;

    int numEdges = 0;
    int numLocalVertices = 0;
    if (world_rank == 0) {
        numEdges = edges.size();
        numLocalVertices = numLocalVertices;
    }
    MPI_Bcast(&numEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //MPI_Scatter(edges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, localEdges.data(), localEdgeCount * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    // scatter the edges to search in them
	int edgesPart = (edges + world_size - 1) / world_size;
	//int* edgeListPart = (int*) malloc(edgesPart * 3 * sizeof(int));
	int* edgeListPart = new int[edgesPart * 3];
	if (isParallel) {
		scatterEdgeList(edges, edgeListPart, edges, &edgesPart);
	} 
	else {
		edgeListPart = edges;
	}
    
    // remove later; this loop for debug purposes
    for (auto e : localEdges)
    {
        std::cout << "world rank: " << world_rank << " with edge: " << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }

    //Subset *subsets = new Subset[numVertices];
    Subset subsets(numVertices);
    int *cheapest = new int[numVertices];

    // // Initialize subsets and cheapest arrays
    // for (int i = 0; i < numVertices; i++) {
    //     subsets[i]->parent = new int[numVertices];
    //     subsets[i]->rank = new int[numVertices];
    //     subsets[i].numElements = numVertices;
    // }
>>>>>>> Stashed changes

    int numEdgesSelectedForMST = 0;
    int *cheapest = new int[numVertices * 3]; //figrue out why * 3
    int *cheapestReceived;

<<<<<<< Updated upstream
    while (numSubtreesRemaining > 1) {
        // iterate through all the edges in the given subset 'localEdges' and 
        // find the cheapest weighted edge for each subset.
        for (int i = 0; i < localEdges.size(); i++) {
            int set1 = find(subsets, localEdges[i].getFirstVertex());
            int set2 = find(subsets, localEdges[i].getSecondVertex());

            if (set1 != set2) {
                if (cheapest[set1] == -1 || localEdges[cheapest[set1]].getWeight() > localEdges[i].getWeight())
                cheapest[set1] = i;
                if (cheapest[set2] == -1 || localEdges[cheapest[set2]].getWeight() > localEdges[i].getWeight())
                cheapest[set2] = i;
            }
        }
        
        // synchronization phase?

        // iterate through all the subsets and if a minimum weighted edge exists for a subset, 
        // we add it to the selectedEdges vector, update the MSTWeight, perform the union of the subsets, and decrement the numTrees.
        // TODO: I think we have to broadcast to all other processes when subtrees decreases?
        for (int v = 0; v < numVertices; v++) {
            if (cheapest[v] != -1) {
                int set1 = find(subsets, localEdges[cheapest[v]].getFirstVertex());
                int set2 = find(subsets, localEdges[cheapest[v]].getSecondVertex());
                if (set1 != set2) {
                    mstEdgesSelected.push_back(localEdges[cheapest[v]]);
                    localMstWeight += edges[cheapest[v]].getWeight();
                    unionSets(subsets, set1, set2);
                    numSubtreesRemaining--;
                }

                cheapest[v] = -1;
            }
        }
        
    }
    
    //since each process will have its own mst weight after unioning everything, will we have to gather the weights? maybe not?
    //maybe send to root?
    
    //if we have the root process, then we print the MST accordingly.
    if (world_rank == 0) {
        //std::cout << "Total Minimum Spanning Tree Weight: " << totalMSTWeight << std::endl;
    }
=======
    if (isParallel) {
        cheapestReceived = new int[numVertices * 3]; // figure out why 3
    }

    for (int i = 1; i < vertices && edgesMST < vertices - 1; i *= 2) {
		// reset all closestEdge
		for (int j = 0; j < vertices; j++) {
			cheapest[j * 3 + 2] = INT_MAX;
		}

		// find closestEdge
		for (int j = 0; j < edgesPart; j++) {
			int* currentEdge = &edgeListPart[j * 3];
			int parent[2] = { findSet(subsets, currentEdge[0]), findSet(subsets, currentEdge[1]) };

			// eventually update closestEdge
			if (parent[0] != parent[1]) {
				for (int k = 0; k < 2; k++) {
					bool closestEdgeNotSet = closestEdge[parent[k]* 3 + 2] == INT_MAX;
					bool weightSmaller = currentEdge[2] < closestEdge[parent[k] * 3	+ 2];
					if (closestEdgeNotSet || weightSmaller) {
						copyEdge(&closestEdge[parent[k] * 3],currentEdge);
					}
				}
			}
		}

		if (isParallel) {
			int from;
			int to;
			for (int step = 1; step < world_size; step *= 2) {
				if (world_rank % (2 * step) == 0) {
					from = world_rank + step;
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
				else if (world_rank % step == 0) {
					to = world_rank - step;
					MPI_Send(closestEdge, vertices * 3, MPI_INT, to,0,MPI_COMM_WORLD);
				}
			}
			// publish all closestEdge parts
			MPI_Bcast(closestEdge, vertices * 3, MPI_INT, 0,MPI_COMM_WORLD);
		}

		// add new edges to MST
		for (int j = 0; j < vertices; j++) {
			if (closestEdge[j * 3 + 2] != INT_MAX) {
				int from = closestEdge[j * 3];
				int to = closestEdge[j * 3 + 1];

				// prevent adding the same edge twice
				if (findSet(set, from) != findSet(set, to)) {
					if (rank == 0) {
						copyEdge(&mst->edgeList[edgesMST * 3],&closestEdge[j * 3]);
					}
					edgesMST++;
					unionSet(set, from, to);
				}
			}
		}
	}
    // clean up
	deleteSet(set);
	free(closestEdge);
	if (parallel) {
		free(closestEdgeRecieved);
		free(edgeListPart);
	}


    // // in the beginning, each process is aware that there are |V| components.
    // // I think this needs to be global...

    // // TODO: num vertices depends on split local edge set
    // int numSubtreesRemaining = numVertices;
    // int localMstWeight = 0;

    // // todo num subtrees remaining locally is 1 or no more edges to connect to other components
    // while (numSubtreesRemaining > 1)
    // {
    //     // iterate through all the edges in the given subset 'localEdges' and
    //     // find the cheapest weighted edge for each subset.
    //     for (int i = 0; i < localEdges.size(); i++)
    //     {
    //         int set1 = find(subsets, localEdges[i].getFirstVertex());
    //         int set2 = find(subsets, localEdges[i].getSecondVertex());

    //         if (set1 != set2)
    //         {
    //             if (cheapest[set1] == -1 || localEdges[cheapest[set1]].getWeight() > localEdges[i].getWeight())
    //                 cheapest[set1] = i;
    //             if (cheapest[set2] == -1 || localEdges[cheapest[set2]].getWeight() > localEdges[i].getWeight())
    //                 cheapest[set2] = i;
    //         }
    //     }

    //     // synchronization phase -- doing the merge

    //     // iterate through all the subsets and if a minimum weighted edge exists for a subset,
    //     // we add it to the selectedEdges vector, update the MSTWeight, perform the union of the subsets, and decrement the numTrees.
        
    //     for (int v = 0; v < numVertices; v++)
    //     {
    //         if (cheapest[v] != -1)
    //         {
    //             int set1 = find(subsets, localEdges[cheapest[v]].getFirstVertex());
    //             int set2 = find(subsets, localEdges[cheapest[v]].getSecondVertex());
    //             if (set1 != set2)
    //             {
    //                 mstEdgesSelected.push_back(localEdges[cheapest[v]]);
    //                 localMstWeight += edges[cheapest[v]].getWeight();
    //                 unionSets(subsets, set1, set2);
    //                 numSubtreesRemaining--;
    //             }

    //             cheapest[v] = -1;
    //         }
    //     }

    //     // TODO: broadcast the localMstWeight to all processes
    //     // 1. even process receives from odd process
    //     // 2. odd process sends from even process

    //     // have barrier so that every loop iteration will be synchronized
    //     MPI_Barrier(MPI_COMM_WORLD);

    //     //TODO: broadcast the (global) number of subtrees remaining
    // }

    // // since each process will have its own mst weight after unioning everything, will we have to gather the weights? maybe not?
    // // maybe send to root?

    // // if we have the root process, then we print the MST accordingly.
    // if (world_rank == 0)
    // {
    //     // std::cout << "Total Minimum Spanning Tree Weight: " << totalMSTWeight << std::endl;
    // }
>>>>>>> Stashed changes
}

int world_rank = 0;
int world_size = 0;

int main() {
    // Initialize the MPI environment
    Graph g = createGraph(10);
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

<<<<<<< Updated upstream
    boruvkasAlgorithmMPI(g.getGraphEdges(), 10, world_rank, world_size);
=======
    if (world_size < 1)
    {
        std::cerr << "Number of processes must be greater than 0" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    // std::cout << "world rank: " << world_rank << " world size: " << world_size << std::endl;
    Graph g;
    if (world_rank == 0) { 
        g.loadGraphFromFile(input_file_path);
    }

    boruvkasAlgorithmMPI(g.getGraphEdges(), g.getNumVertices(), world_rank, world_size);

    if (rank == 0) {
		

		// print the edges of the MST
		printf("Minimum Spanning Tree (Boruvka):\n");
		printWeightedGraph(mst);

		unsigned long weightMST = 0;
		for (int i = 0; i < mst->edges; i++) {
			weightMST += mst->edgeList[i * 3 + 2];
		}

		printf("MST weight: %lu\n", weightMST);
		printf("Time elapsed: %f s\n", MPI_Wtime() - start);
		// cleanup
		deleteWeightedGraph(graph);
		deleteWeightedGraph(mst);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;

>>>>>>> Stashed changes
    return 0;
}