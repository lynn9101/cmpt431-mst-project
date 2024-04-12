// A C++ Program to generate test cases for
// an weighted undirected graph
// Referenece: https://www.geeksforgeeks.org/test-case-generation-set-4-random-directed-undirected-weighted-and-unweighted-graphs/
#include <bits/stdc++.h>
#include "core/utils.h"
#include <string>
#include <iostream>
using namespace std;

// Define the maximum weight of edges
#define MAX_WEIGHT 20

void generateRandomGraph(uint n_vertices) {
    set<pair<uint, uint>> container;
    set<pair<uint, uint>>::iterator it;

    std::string filePath = "graph_inputs/random_" + std::to_string(n_vertices) + ".txt";
    std::cout << "Graph input path : " << filePath << std::endl;
    const char* path = filePath.c_str();
    if (freopen(path, "w", stdout) == NULL) {
        perror("freopen() failed.\n");
        return;
    }
    
    // For random values every time
    // Reference: https://stackoverflow.com/questions/288739/generate-random-numbers-uniformly-over-an-entire-range
    std::random_device rd;
    mt19937 mt(rd());
    std::uniform_real_distribution<double> edge_distribution(0, 1);
    std::uniform_int_distribution<int> weight_distribution(1, MAX_WEIGHT);

    // Start generating graph
    uint n_edges = 0;

    for (uint i = 0; i < n_vertices; i++) {
        for (uint j = i+1; j < n_vertices; j++) {
            // Between any 2 vertices, the edge exists if its distribution probability is under 0.5
            if (edge_distribution(mt) < 0.5) {
                pair<uint, uint> p = std::make_pair(i, j);
                container.insert(p);
                n_edges++;
            }
        }
    }
    // First print the number of vertices and edges
    printf("%d %d\n", n_vertices, n_edges);

    // Then print the edges of the form (a b)
    // where 'a' is connected to 'b'
    for (it=container.begin(); it!=container.end(); ++it) {
        printf("%d %d %d\n", it->first, it->second, weight_distribution(mt));
    }

    container.clear();
    fclose(stdout);
}

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "graph_generator",
        "Generate random input graph used for MST algorithms");
    
    options.add_options(
        "",
        {
            {"nVertices", "Number of Vertices",
                cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_VERTICES)},
        });
    auto cl_options = options.parse(argc, argv);
    uint n_vertices = cl_options["nVertices"].as<uint>();

    std::cout << "Number of Vertices : " << n_vertices << std::endl;
    std::cout << "Generating graph\n";
    generateRandomGraph(n_vertices);

    return(0);
}