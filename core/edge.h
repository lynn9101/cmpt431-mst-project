#ifndef _WEIGHTED_EDGE_H
#define _WEIGHTED_EDGE_H

#include "utils.h"

class Edge {
public:
    Edge(uintV v1, uintV v2, uintE w)
        : vertex1(v1), vertex2(v2), weight(w) {}

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