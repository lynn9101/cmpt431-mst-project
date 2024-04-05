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
    uintE firstEdgeIndex;
    uintE lastEdgeIndex;
    uintE edgeArraySize;
    Edge *edgeSubsetToBeSorted;
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

  std::sort(threadInfo->edgeSubsetToBeSorted, threadInfo->edgeSubsetToBeSorted + threadInfo->edgeArraySize);
  std::cout << "array from " << threadInfo->threadID << " sorted!\n";

  // double timeTaken = t1.stop();
  //timesTakenPerThread[threadInfo->threadID] = timeTaken;
}

void createThreads(uintE numThreads, std::vector<Edge> mainEdgeSet) {
  uintE numEdgesPerThread = mainEdgeSet.size() / numThreads;
  uintE remainderEdges = mainEdgeSet.size() % numThreads;
  uintE runningStartEdgeCounter = 0;
  Edge *intermediateArray;
  for (int i = 0; i < numThreads; i++) {
    threadInformations[i].threadID = i;
    threadInformations[i].firstEdgeIndex = runningStartEdgeCounter;
    if (i == 0) {
      threadInformations[i].lastEdgeIndex = numEdgesPerThread + remainderEdges;
      threadInformations[i].edgeArraySize = numEdgesPerThread + remainderEdges;
      intermediateArray = new Edge[numEdgesPerThread + remainderEdges];
      runningStartEdgeCounter += (numEdgesPerThread + remainderEdges);
    } else {
      threadInformations[i].lastEdgeIndex = numEdgesPerThread;
      threadInformations[i].edgeArraySize = numEdgesPerThread;
      intermediateArray = new Edge[numEdgesPerThread];
      runningStartEdgeCounter += numEdgesPerThread;
    }
    threadInformations[i].timeTaken = 0.0;
    for (int j = 0; j < threadInformations[i].edgeArraySize; j++) {
      intermediateArray[j] = mainEdgeSet[threadInformations[i].firstEdgeIndex + j];
      std::cout << "edge " << threadInformations[i].firstEdgeIndex + j << " added!\n";
    }
    threadInformations[i].edgeSubsetToBeSorted = intermediateArray;
    threads.emplace_back(sortSubsetFunc, &threadInformations[i]);
  }
}

void joinThreads() {
  for (auto &thread : threads) {
    thread.join();
  }
}

int main(int argc, char *argv[]) {
  //TODO: get numThreads as parameter on command line from user.
  uintE numThreads = 4;
  threadInformations = new ThreadInfo[numThreads];
  Graph g(10);
  
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

  g.addEdge(e1);
  g.addEdge(e2);
  g.addEdge(e3);
  g.addEdge(e4);
  g.addEdge(e5);
  g.addEdge(e6);
  g.addEdge(e7);
  g.addEdge(e8);
  g.addEdge(e9);
  g.addEdge(e10);
  g.addEdge(e11);
  g.addEdge(e12);
  g.addEdge(e13);
  g.addEdge(e14);
  g.addEdge(e15);
  g.addEdge(e16);

  threadInformations = new ThreadInfo[numThreads];
  createThreads(numThreads, g.getGraphEdges());
  joinThreads();
  for (int i = 0; i < numThreads; i++) {
    std::cout << "Thread " << i << "'s sorted edge array:" << std::endl;
    for (int j = 0; j < threadInformations[i].edgeArraySize; j++) {
      std::cout << threadInformations[i].edgeSubsetToBeSorted[j].getFirstVertex() << " to " << threadInformations[i].edgeSubsetToBeSorted[j].getSecondVertex() << " with weight " << threadInformations[i].edgeSubsetToBeSorted[j].getWeight() << std::endl;
    }
    std::cout << std::endl << std::endl;
  }
  // merge all of them in step 1

  //std::vector<Edge> res = g.getMST();
  // for (auto e : res) {
  //   std::cout << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
  // }
  return 0;
}