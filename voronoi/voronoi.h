#ifndef VORONOI_H
#define VORONOI_H

#include <list>

#include "geometry/polygon.h"

class Voronoi
{
public:
    Voronoi() = default;
    Voronoi(int width, int height);

    int width;
    int height;
    std::vector<std::shared_ptr<Polygon>> polygons;
    using polygons_const_iterator =
        std::vector<std::shared_ptr<Polygon>>::const_iterator;
    using polygons_iterator = std::vector<std::shared_ptr<Polygon>>::iterator;

    polygons_iterator addPoly(const Polygon&);
    polygons_iterator addPoly(const std::shared_ptr<Polygon>&);
    polygons_iterator erasePoly(const Polygon&);
    polygons_iterator erasePoly(const std::shared_ptr<Polygon>&);
};

#endif  // VORONOI_H
