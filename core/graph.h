#ifndef _WEIGHTED_GRAPH_H
#define _WEIGHTED_GRAPH_H

#include "edge.h"
#include "utils.h"
#include <vector>
#include <algorithm>

// the website/pdf used to understand and adapt the serial version of kruskals is below:
// https://www.geeksforgeeks.org/kruskals-minimum-spanning-tree-algorithm-greedy-algo-2/
// https://www.cs.cmu.edu/afs/cs/academic/class/15210-f14/www/lectures/mst.pdf
// Graph code adapted from assignments 3/4 from earlier in the semester

class UnionFindStructure
{
private:
    int *parent;
    int *rank;

public:
    UnionFindStructure(int numVertices)
    {
        parent = new int[numVertices];
        rank = new int[numVertices];

        for (int i = 0; i < numVertices; i++)
        {
            parent[i] = -1;
            rank[i] = 1;
        }
    }

    ~UnionFindStructure()
    {
        delete parent;
        delete rank;
    }

    // function to detect cycles in the graph
    int find(int i)
    {
        if (parent[i] == -1)
        {
            return i;
        }
        return parent[i] = find(parent[i]);
    }

    void kruskalUnion(uintV firstVertex, uintV secondVertex)
    {
        int firstV = find(firstVertex);
        int secondV = find(secondVertex);

        if (firstV != secondV)
        {
            if (rank[firstV] < rank[secondV])
            {
                parent[firstV] = secondV;
            }
            else if (rank[firstV] > rank[secondV])
            {
                parent[secondV] = firstV;
            }
            else
            {
                parent[secondV] = firstV;
                rank[firstV]++;
            }
        }
    }
};

class Graph
{
private:
    // vector of pairs of an int and another pair; first element of the pair is the int,
    // second element is the pair of vertices which are connected.
    std::vector<Edge> graphEdges;
    int numVertices;
    uintE mstWeight;

public:
    Graph(int V) : numVertices(V), mstWeight(-1) {}

    void addEdge(Edge e)
    {
        if (e.getFirstVertex() < numVertices && e.getSecondVertex() < numVertices)
        {
            graphEdges.push_back(e);
        }
        else
        {
            std::cerr << "One or more of the vertices in this edge don't exist in the graph." << std::endl;
        }
    }

    std::vector<Edge> getGraphEdges()
    {
        return graphEdges;
    }

    int getNumVertices()
    {
        return numVertices;
    }

    void assignEdgeSet(std::vector<Edge> edges)
    {
        graphEdges = edges;
    }

    void sort()
    {
        std::sort(graphEdges.begin(), graphEdges.end());
    }

    uintE getMstWeight()
    {
        if (mstWeight == -1)
        {
            getMST();
        }
        return mstWeight;
    }

    std::vector<Edge> getMST()
    {
        std::vector<Edge> result;
        uintE minPathWeight = 0;
        UnionFindStructure unionFindStructure(numVertices);
        for (Edge e : graphEdges)
        {
            uintE edgeWeight = e.getWeight();
            uintV firstVertex = e.getFirstVertex();
            uintV secondVertex = e.getSecondVertex();

            if (unionFindStructure.find(firstVertex) != unionFindStructure.find(secondVertex))
            {
                unionFindStructure.kruskalUnion(firstVertex, secondVertex);
                minPathWeight += edgeWeight;
                result.push_back(e);
            }
        }
        if (minPathWeight < mstWeight)
        {
            mstWeight = minPathWeight;
        }
        return result;
    }
};

#endif