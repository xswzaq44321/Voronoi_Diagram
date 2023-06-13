#include "point.h"

Point::Point() {}

Point::Point(int x, int y)
    : x(x),
      y(y)
{
}

Point::Point(const Point* old)
    : x(old->x),
      y(old->y)
{
}

Point::Point(const Point& old)
    : x(old.x),
      y(old.y)
{
}

Point::Point(Point&& old)
    : x(old.x),
      y(old.y)
{
}

Point::Point(const PointF* old)
    : x((int) old->x),
      y((int) old->y)
{
}

Point::Point(const PointF& old)
    : x((int) old.x),
      y((int) old.y)
{
}

double Point::distance(const Point& other) const
{
    return sqrt(pow(this->x - other.x, 2) + pow(this->y - other.y, 2));
}

bool Point::operator()(const Point& lhs, const Point& rhs)
{
    return (lhs.x < rhs.x) || (lhs.x == rhs.x && lhs.y < rhs.y);
}

Point& Point::operator=(const Point& rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;
}

bool Point::operator==(const Point& rhs) const
{
    return (this->x == rhs.x && this->y == rhs.y);
}

bool Point::operator<(const Point& rhs) const
{
    return (this->x < rhs.x) || (this->x == rhs.x && this->y < rhs.y);
}

PointF::PointF() {}

PointF::PointF(double x, double y)
    : x(x),
      y(y)
{
}

PointF::PointF(const PointF* old)
    : x(old->x),
      y(old->y)
{
}

PointF::PointF(const PointF& old)
    : x(old.x),
      y(old.y)
{
}

PointF::PointF(const Point* old)
    : x(old->x),
      y(old->y)
{
}

PointF::PointF(const Point& old)
    : x(old.x),
      y(old.y)
{
}

double PointF::distance(const PointF& other) const
{
    return sqrt(pow(this->x - other.x, 2) + pow(this->y - other.y, 2));
}
PointF::operator Point() const
{
    return Point((int) x, (int) y);
}

Point getVector(const Point& a, const Point& b)
{
    return Point(b.x - a.x, b.y - a.y);
}

double cross(const Point& a, const Point& b, const Point& o)
{
    Point v1 = getVector(o, a);
    Point v2 = getVector(o, b);
    return v1.x * v2.y - v1.y * v2.x;
}

PointF midPoint(const std::vector<Point>& points)
{
    double x = 0, y = 0;
    for (const auto& point : points) {
        x += point.x;
        y += point.y;
    }
    return PointF(x / points.size(), y / points.size());
}
