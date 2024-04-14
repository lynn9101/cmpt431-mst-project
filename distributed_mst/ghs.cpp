#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <algorithm>
#include <numeric>
#include <mpi.h>

#include "core/graph.h"
#include "core/edge.h"
#include "core/get_time.h"
#include "core/subset.h"

// Reference: https://www.tutorialspoint.com/boruvka-s-algorithm-for-minimum-spanning-tree-in-cplusplus

struct SubSet
{
    int parent;
    int rank;
};

// Find parent of a node
int find(Subset subsets[], int i)
{
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

// Function to perform union of two subsets using union by rank
void unionSets(Subset subsets[], int x, int y)
{
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);

    if (subsets[xroot].rank < subsets[yroot].rank)
    {
        subsets[xroot].parent = yroot;
    }
    else if (subsets[xroot].rank > subsets[yroot].rank)
    {
        subsets[yroot].parent = xroot;
    }
    else
    {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

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

// each process starts at 1 vertex
// w/ its own edges
void distributedMST(int np, int pid, std::vector<Edge> local_edges)
{
    // init local components
}

/*
NOTE -- asumming each process is a vertex

each process knows
 - its own edges
 - number of vertices (processes) total
*/
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
    int world_rank = 0;
    int world_size = 0;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    std::vector<Edge> local_edges;
    readGraphAndDistributeEdges(input_file_path, world_size, world_rank, local_edges);

    // wait till all processes have their respective edges
    MPI_Barrier(MPI_COMM_WORLD);

    // run distributed mst
    distributedMST(world_size, world_rank, local_edges);

    // TODO: begin boruvka
    MPI_Finalize();

    return 0;
}