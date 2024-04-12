#ifndef _WEIGHTED_EDGE_H
#define _WEIGHTED_EDGE_H

#include "utils.h"

// Undirected, weighted edge
class Edge {
public:
    Edge() : vertex1(0), vertex2(0), weight(0) {}
    Edge(uintV v1, uintV v2, uintE w)
        : vertex1(v1), vertex2(v2), weight(w) {}
    Edge(const Edge& other)
        : vertex1(other.vertex1), vertex2(other.vertex2), weight(other.weight) {}

    uintV getFirstVertex() { return vertex1; }
    uintV getSecondVertex() { return vertex2; }
    uintE getWeight() const { return weight; }

private:
    uintV vertex1;
    uintV vertex2;
    uintE weight;
};

bool operator<(const Edge& e1, const Edge& e2) {
    return e1.getWeight() < e2.getWeight();
}
bool operator>(const Edge& e1, const Edge& e2) {
    return e1.getWeight() > e2.getWeight();
}

#endif