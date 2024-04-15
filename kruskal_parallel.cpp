#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <algorithm>

#include "core/graph.h"
#include "core/edge.h"
#include "core/get_time.h"

class ThreadInfo {
public:
    int threadID;
    uintE edgeArraySize;
    std::vector<Edge> edgeSubsetToBeSorted;
    double timeTaken;
};

std::vector<std::thread> threads;
ThreadInfo *threadInformations;

// pseudo for filter-kruskals...?
/*
Procedure filterKruskal(E, T: Sequence of Edge, P: UnionFind)
    // Check if the number of edges is less than or equal to a threshold (for parallelizing, can this be the number of edges?)
    if m is less than or equal to the threshold determined by kruskalThreshold(n, |E|, |T|) then
        // If the condition is met, apply regular Kruskal's algorithm
        kruskal(E, T, P)
    else
        // Otherwise, proceed with the filtering variant of Kruskal's algorithm
        // Step 1: Pick a pivot edge 'p' from the set of edges 'E'
        pick a pivot p from the set of edges E
        
        // Step 2: Partition the set of edges 'E' into two subsets based on the pivot edge 'p'
        E≤ := { edges in E with weights less than or equal to the weight of pivot edge 'p' }
        E> := { edges in E with weights greater than the weight of pivot edge 'p' }
        
        // Step 3: Recursively apply filterKruskal on each subset
        filterKruskal(E≤, T, P)
        filterKruskal(E>, T, P)
        
        // Step 4: Filter the set 'E>' using the UnionFind data structure 'P'
        E> := filter(E>, P)
        // The 'filter' function returns edges from 'E>' such that their endpoints are in different components of 'P'
*/
// sorts each individual thread's partition of the entire edge set.
void sortSubsetFunc(void *_threadInfo) {
    ThreadInfo* threadInfo = static_cast<ThreadInfo*>(_threadInfo);

    // timer t1;
    // t1.start();

    std::sort(threadInfo->edgeSubsetToBeSorted.begin(), threadInfo->edgeSubsetToBeSorted.end());

    // double timeTaken = t1.stop();
    //timesTakenPerThread[threadInfo->threadID] = timeTaken;
}

void createThreads(uintE numThreads, std::vector<Edge> mainEdgeSet) {
    std::vector<Edge> firstHalf(mainEdgeSet.begin(), mainEdgeSet.begin() + (mainEdgeSet.size() / 2));
    std::vector<Edge> secondHalf(mainEdgeSet.begin() + (mainEdgeSet.size() / 2), mainEdgeSet.end());

    uintE numEdgesPerThread = mainEdgeSet.size() / numThreads;
    uintE remainderEdges = mainEdgeSet.size() % numThreads;
    threadInformations[0].edgeArraySize = numEdgesPerThread + remainderEdges;
    threadInformations[0].edgeSubsetToBeSorted = firstHalf;
    threadInformations[0].threadID = 0;
    threadInformations[0].timeTaken = 0.0;
    threads.emplace_back(sortSubsetFunc, &threadInformations[0]);
    threadInformations[1].edgeArraySize = numEdgesPerThread;
    threadInformations[1].edgeSubsetToBeSorted = secondHalf;
    threadInformations[1].threadID = 1;
    threadInformations[1].timeTaken = 0.0;
    threads.emplace_back(sortSubsetFunc, &threadInformations[1]);
}

void joinThreads() {
    for (auto &thread : threads) {
        thread.join();
    }
}

std::vector<Edge> mergeSubsets() {
    std::vector<Edge> result;
    uint firstHalfArrayIndex = 0;
    uint secondHalfArrayIndex = 0;
    while (firstHalfArrayIndex < threadInformations[0].edgeArraySize && secondHalfArrayIndex < threadInformations[1].edgeArraySize) {
        if (threadInformations[0].edgeSubsetToBeSorted[firstHalfArrayIndex].getWeight() < threadInformations[1].edgeSubsetToBeSorted[secondHalfArrayIndex].getWeight()) {
            result.push_back(threadInformations[0].edgeSubsetToBeSorted[firstHalfArrayIndex]);
            firstHalfArrayIndex++;
        } else {
          result.push_back(threadInformations[1].edgeSubsetToBeSorted[secondHalfArrayIndex]);
          secondHalfArrayIndex++;
        }
    }
    while (firstHalfArrayIndex < threadInformations[0].edgeArraySize) {
        result.push_back(threadInformations[0].edgeSubsetToBeSorted[firstHalfArrayIndex]);
        firstHalfArrayIndex++;
    }

    while (secondHalfArrayIndex < threadInformations[1].edgeArraySize) {
        result.push_back(threadInformations[1].edgeSubsetToBeSorted[secondHalfArrayIndex]);
        secondHalfArrayIndex++;
    }

    return result;
}

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
        "spanning_tree_parallel",
        "Calculate minimum spanning using parallel execution");

    options.add_options(
        "",
        {
            {"nThreads", "Number of Threads",
                cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_THREADS)},
            {"inputFile", "Input graph file path",
                cxxopts::value<std::string>()->default_value(
                  "./graph_inputs/random_10.txt")},
        });
    auto cl_options = options.parse(argc, argv);
    uint n_threads = cl_options["nThreads"].as<uint>();
    std::string input_file_path = cl_options["inputFile"].as<std::string>();

    std::cout << "Number of Threads : " << n_threads << std::endl;
    if (n_threads < 1) {
        std::cout << "Error: Number of threads or number of iterations must be at least or greater than 1.\n";
        return 0;
    }

    Graph g;
    g.loadGraphFromFile(input_file_path);
    std::cout << "Created graph\n";

    timer t1;
    t1.start();
    threadInformations = new ThreadInfo[n_threads];
    createThreads(n_threads, g.getGraphEdges());
    joinThreads();
    g.assignEdgeSet(mergeSubsets());
    
    // i think merging needs to be done serially
    // merge all of them in step 1, maybe using 2 threads by default?
    MinSpanningTree mst = get_minimum_spanning_tree_serial(g);
    int time = t1.stop();
    std::vector<Edge> resultTreeEdges = mst.tree;
    std::cout << "Minimum spanning tree\n";
    for (auto e : resultTreeEdges) {
      std::cout << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
    }
    std::cout << "Sum of weights in MST : " << mst.sumWeight << std::endl;
    std::cout << "Time taken (in seconds) : " << time << std::endl;
    return 0;
}