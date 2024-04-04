#include <iomanip>
#include <iostream>
#include <stdlib.h>

#include "core/graph.h"
#include "core/edge.h"

class ThreadInfo {
  public:
    int threadID;
    uintV startVertex;
    uintV endVertex;
    int numIterations;
    Graph *g;
    double timeTaken;
};

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

int main(int argc, char *argv[]) {
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

  std::vector<Edge> res = g.getMST();
  for (auto e : res) {
    std::cout << e.getFirstVertex() << " to " << e.getSecondVertex() << " with weight " << e.getWeight() << std::endl;
  }
  return 0;
}