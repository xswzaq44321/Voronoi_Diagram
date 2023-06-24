#include "voronoi.h"

#include <algorithm>

Voronoi::Voronoi(int width, int height)
    : width(width),
      height(height)
{
}

Voronoi::polygons_iterator Voronoi::addPoly(const Polygon& poly)
{
    return polygons.insert(polygons.end(), std::make_shared<Polygon>(poly));
}

Voronoi::polygons_iterator Voronoi::addPoly(
    const std::shared_ptr<Polygon>& poly_ptr)
{
    return polygons.insert(polygons.end(), poly_ptr);
}

Voronoi::polygons_iterator Voronoi::erasePoly(const Polygon& poly)
{
    auto it = std::find_if(
        polygons.begin(), polygons.end(),
        [&poly](const std::shared_ptr<Polygon>& e) { return *e == poly; });
    return polygons.erase(it);
}

Voronoi::polygons_iterator Voronoi::erasePoly(
    const std::shared_ptr<Polygon>& poly_ptr)
{
    auto it = std::find(polygons.begin(), polygons.end(), poly_ptr);
    return polygons.erase(it);
}
