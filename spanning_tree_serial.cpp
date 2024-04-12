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

// the website/pdf used to understand and adapt the serial version of kruskals is below:
// https://www.geeksforgeeks.org/kruskals-minimum-spanning-tree-algorithm-greedy-algo-2/
// https://www.cs.cmu.edu/afs/cs/academic/class/15210-f14/www/lectures/mst.pdf
// Graph code adapted from assignments 3/4 from earlier in the semester
class UnionFindStructure {
private:
    int *parent;
    int *rank;
public:
    UnionFindStructure(int numVertices) {
        parent = new int[numVertices];
        rank = new int[numVertices];

        for (int i = 0; i < numVertices; i++) { 
            parent[i] = -1; 
            rank[i] = 1; 
        }
    }

    ~UnionFindStructure() {
        delete parent;
        delete rank;
    }
    
    //function to detect cycles in the graph
    int find(int i) {
        if (parent[i] == -1) {
            return i;
        }
        return parent[i] = find(parent[i]);
    }

    void kruskalUnion(uintV firstVertex, uintV secondVertex) {
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

MinSpanningTree get_minimum_spanning_tree_serial(Graph g) {
    std::vector<Edge> result;
    uintE minPathWeight = 0;
    UnionFindStructure unionFindStructure(g.getNumVertices());

    for (Edge e : g.getGraphEdges()) {
        uintE edgeWeight = e.getWeight();
        uintV firstVertex = e.getFirstVertex();
        uintV secondVertex = e.getSecondVertex();

        if (unionFindStructure.find(firstVertex) != unionFindStructure.find(secondVertex)) { 
            unionFindStructure.kruskalUnion(firstVertex, secondVertex); 
            minPathWeight += edgeWeight; 
            result.push_back(e); 
        }
    }

    return {result, minPathWeight};
}

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "spanning_tree_serial",
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
    MinSpanningTree mst = get_minimum_spanning_tree_serial(g);
    std::vector<Edge> resultTreeEdges = mst.tree;
    std::cout << "Minimum spanning tree\n";
    for (auto e : resultTreeEdges) {
        std::cout << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }
    std::cout << "Sum of weights in MST : " << mst.sumWeight << std::endl;
    std::cout << "Time taken (in seconds) : " << t1.stop() << std::endl;

    return 0;
}