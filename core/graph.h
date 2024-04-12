#ifndef _WEIGHTED_GRAPH_H
#define _WEIGHTED_GRAPH_H

#include "edge.h"
#include "utils.h"
#include <vector>
#include <algorithm>
#include <iostream>

class Graph {
private:
    // vector of pairs of an int and another pair; first element of the pair is the int, 
    // second element is the pair of vertices which are connected.
    std::vector<Edge> graphEdges;
    int numVertices;
    
public:
    Graph() = default;
    Graph(int V) : numVertices(V) {}
    Graph(int V, std::vector<Edge> edgesVec) : numVertices(V), graphEdges(edgesVec) {}

    void addEdge(Edge e) {
        if (e.getFirstVertex() < numVertices && e.getSecondVertex() < numVertices) {
            graphEdges.push_back(e);
        } else {
            std::cerr << "One or more of the vertices in this edge don't exist in the graph." << std::endl;
        }
    }

    std::vector<Edge> getGraphEdges() {
        return graphEdges;
    }

    void assignEdgeSet(std::vector<Edge> edges) {
        graphEdges = edges;
    }

    void sort() {
        std::sort(graphEdges.begin(), graphEdges.end());
    }

    void printGraph() {
        std::cout << "Number of Vertices : " << numVertices << std::endl;
        std::cout << "Printing Graph..." << std::endl;
        for (Edge e : graphEdges) {
            std::cout << "(" << e.getFirstVertex() << ", " << e.getSecondVertex() << ") = " << e.getWeight() << std::endl;
        }
    }

    void loadGraphFromFile(const std::string& fileName) {

    }
};

#endif