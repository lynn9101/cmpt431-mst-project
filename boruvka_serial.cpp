#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include "core/graph.h"
#include "core/edge.h"
#include "core/get_time.h"
#include "core/utils.h"

struct MinSpanningTree {
    std::vector<Edge> tree;
    uintE sumWeight;
};

class UnionFindStructure {
private:
    int *parent;
    int *rank;
public:
    UnionFindStructure(int numVertices) {
        parent = new int[numVertices];
        rank = new int[numVertices];

        for (int i = 0; i < numVertices; i++) { 
            parent[i] = i; 
            rank[i] = 0; 
        }
    }

    ~UnionFindStructure() {
        delete parent;
        delete rank;
    }
    
    //function to detect cycles in the graph
    int find(int i) {
        if (parent[i] == i) {
            return i;
        }
        return parent[i] = find(parent[i]);
    }

    void boruvkaUnion(uintV firstVertex, uintV secondVertex) {
        int firstV = find(firstVertex);
        int secondV = find(secondVertex);

        if (firstV != secondV) { 
            if (rank[firstV] < rank[secondV]) { 
                parent[firstV] = secondV; 
            } 
            else if (rank[firstV] > rank[secondV]) { 
                parent[secondV] = firstV; 
            } 
            else { 
                parent[secondV] = firstV; 
                rank[firstV]++; 
            } 
        }
    }
};

// Sequential Boruvka Algorithm
MinSpanningTree sequentialBoruvka(Graph g) {
    std::vector<Edge> result;
    std::vector<Edge> minimum;
    for (int i = 0; i < g.getNumVertices(); i++) {
        Edge e(-1, -1, -1);
        minimum.push_back(e);
    }
    uintE minPathWeight = 0;
    UnionFindStructure unionFindStructure(g.getNumVertices());

    // Initially there are V different tree with single vertex.
    uintV n_trees = g.getNumVertices();

    while (n_trees > 1) {
        // Step 1: Find minimum edge for each component
        for (Edge e : g.getGraphEdges()) {
            uintE edgeWeight = e.getWeight();
            uintV firstVertex = e.getFirstVertex();
            uintV secondVertex = e.getSecondVertex();

            uintV component1 = unionFindStructure.find(firstVertex);
            uintV component2 = unionFindStructure.find(secondVertex);

            if (component1 != component2) {
                if (minimum[component1].getWeight() == -1 || minimum[component1].getWeight() > edgeWeight)
                    minimum[component1] = e;
                if (minimum[component2].getWeight() == -1 || minimum[component2].getWeight() > edgeWeight)
                    minimum[component2] = e;
            }
        }

        // Step 2: Merge components
        for (int v = 0; v < g.getNumVertices(); ++v) {
            if (minimum[v].getWeight() != -1) {
                uintE edgeWeight = minimum[v].getWeight();
                uintV firstVertex = minimum[v].getFirstVertex();
                uintV secondVertex = minimum[v].getSecondVertex();

                uintV component1 = unionFindStructure.find(firstVertex);
                uintV component2 = unionFindStructure.find(secondVertex);

                if (component1 != component2) {
                    // Add edge to the MST
                    minPathWeight += edgeWeight;
                    unionFindStructure.boruvkaUnion(component1, component2);

                    result.push_back(minimum[v]);
                    n_trees--;
                }
            }
        }

        // Step 3: Reset minmum array
        for (int i = 0; i < g.getNumVertices(); i++) {
            Edge e(-1, -1, -1);
            minimum[i] = e;
        }
    }

    return {result, minPathWeight};
}

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "boruvka_serial",
        "Calculate minimum spanning using serial execution");

    options.add_options(
        "",
        {
            {"inputFile", "Input graph file path",
                cxxopts::value<std::string>()->default_value(
                    "./graph_inputs/random_10.txt")},
        });

    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();

    Graph g;
    g.loadGraphFromFile(input_file_path);
    std::cout << "Created graph\n";

    timer t1;
    t1.start();
    g.sort();
    MinSpanningTree mst = sequentialBoruvka(g);
    std::vector<Edge> resultTreeEdges = mst.tree;
    std::cout << "Minimum spanning tree\n";
    for (auto e : resultTreeEdges) {
        std::cout << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }
    std::cout << "Sum of weights in MST : " << mst.sumWeight << std::endl;
    std::cout << "Time taken (in seconds) : " << t1.stop() << std::endl;

    return 0;
}