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
- ✅ initialize the world size to however many vertices there are.
    - in boruvka's algorithm, we have |V| "components". So therefore, the MPI world size
    must be the number of vertices to begin the algorithm. (implement an error check)
- each process(component) then will find the smallest edge which connects it to another component.
- We then merge the two components, and then repeat the above steps until we only have one complete component.

Thought of steps, adapted from the link mentioned above (can be changed if some are found to be wrong)
1. Initialize MPI environment. ✅
2. Setting up data & structures for processes
    1. divide up vertices among processes -> each process is its own vertice
    2. distribute edges based on the vertices to the processes ✅
3. Initialize local data structures (subsets, cheapest edges).
4. while (multiple connected components exist - more than one component altogether):
    Each process computes the cheapest edges for its vertices locally.

    Exchange information about cheapest edges with other processes. (probably done via broadcast?) - look at friday.

    Update local data structures based on received information. DONE...?

    Check for termination condition (if there is one component left)

9. Gather MST edges from all processes and construct the final MST.

10. Output the final MST.

11. Finalize MPI environment.

*/

// Function to find the subset of an element using path compression
int find(Subset subsets[], int vertex)
{
    if (subsets[vertex].parent != vertex)
        subsets[vertex].parent = find(subsets, subsets[vertex].parent);
    return subsets[vertex].parent;
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

// distributes edges based on vertices' outgoing edges for a process
void readGraphAndDistributeEdges(std::string input_file_path, int np, int pid, std::vector<Edge> &local_edges)
{
    if (pid == 0)
    {
        Graph g;
        std::vector<Edge> edges;
        std::map<int, std::vector<Edge>> vertex_out_edges;
        int numEdges_per_process[np] = {0};
        g.loadGraphFromFile(input_file_path);
        edges = g.getGraphEdges();

        if (np < g.getNumVertices())
        {
            std::cerr << "Number of processes must be equal to the number of vertices" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // figure out vertex out edges, ensure edges are sorted
        std::sort(edges.begin(), edges.end(),
                  [](Edge a, Edge b)
                  {
                      return a.getFirstVertex() < b.getFirstVertex() ||
                             (a.getFirstVertex() == b.getFirstVertex() && a.getSecondVertex() < b.getSecondVertex());
                  });

        for (Edge e : edges)
        {
            vertex_out_edges[e.getFirstVertex()].emplace_back(e);
            vertex_out_edges[e.getSecondVertex()].emplace_back(e);
            numEdges_per_process[e.getFirstVertex()]++;
            numEdges_per_process[e.getSecondVertex()]++;
        }

        // send edges a size message
        for (int i = 1; i < np; i++)
        {
            MPI_Send(&numEdges_per_process[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < np; i++)
        {
            MPI_Send(vertex_out_edges[i].data(), numEdges_per_process[i] * sizeof(Edge), MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }
        local_edges = vertex_out_edges[0];
    }
    else
    {
        // receive edge size message
        int numEdges;
        MPI_Recv(&numEdges, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_edges.resize(numEdges);
        MPI_Recv(local_edges.data(), numEdges * sizeof(Edge), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void boruvkasAlgorithmMPI(int np, int pid, std::vector<Edge> &local_edges)
{
    int numTreesRemaining = np;
    Subset subsets[np];
    int cheapestEdges[np];

    // Initialize subsets and cheapest arrays
    for (int v = 0; v < np; v++)
    {
        subsets[v].parent = v;
        subsets[v].rank = 0;
        cheapestEdges[v] = -1;
    }

    while (numTreesRemaining > 1)
    {
        // 1. figure out cheapest edges out of my component
        for (int e = 0; e < local_edges.size(); e++)
        {
            int v1 = find(subsets, local_edges[e].getFirstVertex());
            int v2 = find(subsets, local_edges[e].getSecondVertex());
            if (v1 != v2)
            {
                if (cheapestEdges[v1] == -1 || local_edges[e].getWeight() < cheapestEdges[v1])
                {
                    cheapestEdges[v1] = e;
                }
                if (cheapestEdges[v2] == -1 || local_edges[e].getWeight() < cheapestEdges[v2])
                {
                    cheapestEdges[v2] = e;
                }
            }
        }

        std::cout << "Process " << pid << " has cheapest edges: " << cheapestEdges[pid] << std::endl;
        break;
        // 2. exchange information about cheapest edges with other processes
        // for (int v = 0; v < np; v++)
        // {
        //     if (v != pid)
        //     {
        //         MPI_ISend(cheapestEdges, np, MPI_INT, v, 0, MPI_COMM_WORLD, &request);
        //         MPI_Wait(&request, &status);
        //     }
        // }

        // 3. update local data structures based on received information
        //    -

        // 4. check for termination condition
    }
}

int main(int argc, char **argv)
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

    // read graph & distribute edges
    std::vector<Edge> local_edges;
    readGraphAndDistributeEdges(input_file_path, np, pid, local_edges);

    // boruvka's algorithm
    boruvkasAlgorithmMPI(np, pid, local_edges);

    MPI_Finalize();

    boruvkasAlgorithmMPI(g.getGraphEdges(), g.getNumVertices(), world_rank, world_size);
    return 0;
}