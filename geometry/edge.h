#ifndef EDGE_H
#define EDGE_H

#include <algorithm>
#include <memory>
#include <stdexcept>

#include "point.h"

class Edge
{
public:
    Edge() = default;
    Edge(const Point& a, const Point& b);
    Edge(const Edge& other) = default;
    Edge& operator=(const Edge& other) = default;
    Edge& operator=(Edge&& other) = default;
    ~Edge() = default;

    // edge defined as from point a to point b
    std::shared_ptr<Point> a;
    std::shared_ptr<Point> b;

    double distance(const Point& other) const;
};

#endif  // EDGE_H
