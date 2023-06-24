#include "edge.h"

Edge::Edge(const Point& a, const Point& b)
    : a(std::make_shared<Point>(a)),
      b(std::make_shared<Point>(b))
{
}

double Edge::distance(const Point& other) const
{
    if (!a || !b)
        throw std::invalid_argument("Edge::Distance: either a or b is null");
    double bar = abs((b->y - a->y) * other.x - (b->x - a->x) * other.y +
                     b->x * a->y - b->y * a->x);
    double bar2 = sqrt(pow(b->y - a->y, 2) + pow(b->x - a->x, 2));
    return bar / bar2;
}
