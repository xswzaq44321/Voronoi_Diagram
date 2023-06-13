#ifndef EDGE_H
#define EDGE_H

#include <algorithm>
#include <memory>
#include <stdexcept>

#include "point.h"

class Edge
{
public:
    Edge();
    Edge(Point a, Point b);
    Edge(const Edge& other) = default;
    Edge(Edge&& other) = default;
    Edge& operator=(const Edge& other) = default;
    Edge& operator=(Edge&& other) = default;
    ~Edge() = default;

    // edge defined as from point a to point b
    Point a;
    Point b;
    bool isComplete;

    double distance(const Point& other) const;
};

#endif  // EDGE_H
