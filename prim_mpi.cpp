#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <mpi.h>
#include <stdio.h>

#include "core/graph.h"
#include "core/edge.h"
#include "core/get_time.h"
#include "core/utils.h"

uint n_vertices;
FILE *input_file;

int* displacement;
int* n_vertices_per_proc;
int* recv_matrix;

int* minimum_tree;
int minTotalWeight;

struct {
    int minWeight;
    int rowRank;    // show which process is assigned to that row
} minRow, row;

int main(int argc, char *argv[]) {
    int num_processes;
    int process_rank;

    timer t1;
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);   // process ID

    // Initialize command line arguments
    cxxopts::Options options(
        "mpi_prim",
        "Calculate minimum spanning using distributed execution");

    options.add_options(
        "",
        {
            {"inputFile", "Input graph file path",
                cxxopts::value<std::string>()->default_value(
                  "./graph_inputs/random_10.txt")},
        });
    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();

    t1.start();
    if (process_rank == 0) {
        const char* file_ctr = input_file_path.c_str();
        input_file = fopen(file_ctr, "r");
        if (input_file) {
            
            int res = fscanf(input_file, "%d", &n_vertices);
            if (res != 1){
                printf("Error reading file\n");
                return 1;
            }
            
        }
        printf("Number of processes : %d\n", num_processes);
    }

    // Root process broadcasts total number of vertices to all other processes
    MPI_Bcast(&n_vertices, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Split the number of vertices to all processes
    n_vertices_per_proc = (int*)malloc(num_processes * sizeof(int));
    displacement = (int*)malloc(num_processes * sizeof(int));

    displacement[0] = 0;
    n_vertices_per_proc[0] = n_vertices / num_processes;    // root process
    int remaining = n_vertices - (n_vertices_per_proc[0] * num_processes);

    for (int i = 1; i < num_processes; i++) {
        if (i < remaining) {
            n_vertices_per_proc[i] = n_vertices_per_proc[0];
        } else {
            n_vertices_per_proc[i] = n_vertices_per_proc[0] + 1;
        }
        displacement[i] = displacement[i-1] + n_vertices_per_proc[i-1];
    }

    // Read the vertices and the weights into an adjacency matrix
    int* matrix;
    if (process_rank == 0) {
        matrix = (int*)malloc(n_vertices * n_vertices * sizeof(int));
        Graph g;
        g.loadGraphFromFile(input_file_path);
        std::vector<Edge> graph_edges = g.getGraphEdges();

        for (int i = 0; i < n_vertices*n_vertices; i++) {
            matrix[i] = 0;
        }
        // Update adjacency matrix with edge weights;
        for (auto& e : graph_edges) {
            int v1 = e.getFirstVertex();
            int v2 = e.getSecondVertex();
            int w = e.getWeight();

            matrix[v1 * n_vertices + v2] = w;                                           
            matrix[v2 * n_vertices + v1] = w;
        }

        // For debugging purpose
        // for (int i = 0; i < n_vertices; i++) {
        //     for (int j = i+1; j < n_vertices; j++) {
        //         //printf("matrix(%d, %d) = %d\n", i, j, matrix[i * n_vertices + j]);
        //     }
        // }

        //printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }

    // Declare a new data type for the matrix
    MPI_Datatype matrixType;
    MPI_Type_contiguous(n_vertices, MPI_INT, &matrixType); // type holds N elements, N = number of vertices
    MPI_Type_commit(&matrixType);
    
    // Root sends chunks of the matrix to different processes
    recv_matrix = (int*)malloc(n_vertices_per_proc[process_rank] * n_vertices * sizeof(int));   // each process will take some rows
    MPI_Scatterv(matrix, n_vertices_per_proc, displacement, matrixType, recv_matrix, n_vertices_per_proc[process_rank], matrixType, 0, MPI_COMM_WORLD);
    
    // debugging
    // if (process_rank == 0) {
    //     for (int i = 0; i < n_vertices; i++) {
    //         for (int j = i+1; j < n_vertices; j++) {
    //             printf("matrix(%d, %d) = %d\n", i, j, recv_matrix[i * n_vertices + j]);
    //         }
    //     }
    // }

    // Each process needs to find its own minimum tree given the set of vertices (rows)
    minimum_tree = (int*)malloc(n_vertices * sizeof(int)); // each vertex i associated with another vertex that forms edge minimum weight
    for (int i = 0; i < n_vertices; i++) {
        minimum_tree[i] = -1; // initialize negative vertex before finiding the minimum edge
    }

    minimum_tree[0] = 0;
    minTotalWeight = 0;

    int min;
    int v1, v2 = 0;

    struct {
        int minWeight;
        int rowRank;    // show which process is assigned to that row
    } minRow, row;

    // Start from vertex 1
    for (int v = 0; v < n_vertices-1; v++) {
        min = INT_MAX;
        for (int i = 0; i < n_vertices_per_proc[process_rank]; i++) {
            if (minimum_tree[i + displacement[process_rank]] != -1) {   
                for (int j = 0; j < n_vertices; j++) {
                    if (minimum_tree[j] == -1) {    // not found assocaited vertex yet
                        if (recv_matrix[i * n_vertices + j] < min && recv_matrix[i * n_vertices + j] != 0) {    // 2nd condition checks if not vertex 0
                            min = recv_matrix[i * n_vertices + j];
                            v1 = i; // row
                            v2 = j; // colume;
                        }
                    }
                }
            }
        }
        row.minWeight = min;  // minimum weight of that row
        row.rowRank = process_rank;
        MPI_Allreduce(&row, &minRow, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD); // get location of minimum weight in that row

        struct {int v1; int v2;} edge;
        edge.v1 = v1 + displacement[process_rank];
        edge.v2 = v2;
        MPI_Bcast(&edge, 1, MPI_2INT, minRow.rowRank, MPI_COMM_WORLD);

        minimum_tree[edge.v2] = edge.v1;
        minTotalWeight += minRow.minWeight;
    }
    double total_time_taken = t1.stop();

    if (process_rank == 0) {
        printf("Minimum spanning tree\n");
        for (int i = 1; i < n_vertices; i++) {
            printf("%d to %d with weight %d\n", i, minimum_tree[i], matrix[i * n_vertices + minimum_tree[i]]);
        }
        printf("Sum of weights in MST : %d\n", minTotalWeight);
        std::cout << "Time taken (in seconds) : " << std::setprecision(TIME_PRECISION) << total_time_taken << "\n";
    }


    MPI_Finalize();
    return 0;
}