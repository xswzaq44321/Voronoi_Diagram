#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <vector>

class PointF;

class Point
{
public:
    Point();
    Point(int x, int y);
    Point(const Point* old);
    Point(const Point& old);
    Point(Point&& old);
    Point(const PointF* old);
    Point(const PointF& old);
    ~Point() = default;

    int x;
    int y;

    double distance(const Point& other) const;
    explicit operator PointF() const;
    bool operator()(const Point& lhs, const Point& rhs);
    Point& operator=(const Point& rhs);
    bool operator==(const Point& rhs) const;
    bool operator<(const Point& rhs) const;
};

class PointF
{
public:
    PointF();
    PointF(double x, double y);
    PointF(const PointF* old);
    PointF(const PointF& old);
    PointF(const Point* old);
    PointF(const Point& old);
    ~PointF() = default;

    double x;
    double y;

    double distance(const PointF& other) const;
    explicit operator Point() const;
};

/**
 * @brief getVector calculates vector form a to b
 * @param a start point
 * @param b end point
 * @return a vector from a to b
 */
Point getVector(const Point& a, const Point& b);

/**
 * @brief cross calculates cross value of points a, b, o
 * @param a
 * @param b
 * @param o
 * @return cross value of vector oa x ob
 */
double cross(const Point& a, const Point& b, const Point& o);
PointF midPoint(const std::vector<Point>& points);

#endif  // POINT_H
