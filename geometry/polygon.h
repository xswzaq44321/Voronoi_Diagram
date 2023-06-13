#ifndef POLYGON_H
#define POLYGON_H

#include <set>
#include <vector>

#include "edge.h"
#include "point.h"

#define PI (3.1415926535)

class Edge;
class Point;

class Polygon
{
public:
    Polygon();
    Polygon(const Polygon& other);
    Polygon(Point f);
    Polygon(double focusx, double focusy);
    ~Polygon() = default;

    std::vector<Edge> edges;
    std::vector<Point> vertices;
    Point focus;

    bool contains(const Point& other);
    bool contains(const int x, const int y);
    void organize();
    void unOrganize();
    bool isComplete();

private:
    bool organized;
};

template <typename It1, typename It2>
void pairsort(It1 first, It1 last, It2 pFirst);
template <typename It1, typename It2, class Comp>
void pairsort(It1 first, It1 last, It2 pFirst, Comp comp);

#endif  // POLYGON_H
