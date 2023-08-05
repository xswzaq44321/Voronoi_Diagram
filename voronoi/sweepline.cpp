#include "sweepline.h"

#include <QDebug>

// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void) (msg), (exp)))

/**
 * @brief calculate determinant
 * @param m 3 by 3 matrix of type double[3][3]
 * @return determinant
 */
double det(double m[3][3])
{
    return (m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] +
            m[0][2] * m[1][0] * m[2][1]) -
           (m[0][2] * m[1][1] * m[2][0] + m[0][1] * m[1][0] * m[2][2] +
            m[0][0] * m[1][2] * m[2][1]);
}

double distance(const PointF& a, const PointF& b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

/**
 * @brief cross calculates cross product of oa cross ob
 * @param o orig point
 * @param a first vertex
 * @param b second vertex
 * @return value of oa cross ob
 */
double cross(const PointF& o, const PointF& a, const PointF& b)
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

/**
 * @brief find circumcenter of three points by the following formula
 * https://zh.wikipedia.org/wiki/%E5%A4%96%E6%8E%A5%E5%9C%93#%E4%B8%89%E8%A7%92%E5%BD%A2%E7%9A%84%E5%A4%96%E6%8E%A5%E5%9C%93
 * @return PointF representing the circumcenter
 */
PointF findCircumcenter(const PointF& r1, const PointF& r2, const PointF& r3)
{
    double X[3][3] = {{pow(r1.x, 2) + pow(r1.y, 2), r1.y, 1},
                      {pow(r2.x, 2) + pow(r2.y, 2), r2.y, 1},
                      {pow(r3.x, 2) + pow(r3.y, 2), r3.y, 1}};
    double Y[3][3] = {{r1.x, pow(r1.x, 2) + pow(r1.y, 2), 1},
                      {r2.x, pow(r2.x, 2) + pow(r2.y, 2), 1},
                      {r3.x, pow(r3.x, 2) + pow(r3.y, 2), 1}};
    double D[3][3] = {{r1.x, r1.y, 1}, {r2.x, r2.y, 1}, {r3.x, r3.y, 1}};
    double DX = det(X);
    double DY = det(Y);
    double DD = det(D);
    assertm(DD != 0, "three points are collinear, can't find circumcenter");
    return PointF(DX / (2 * DD), DY / (2 * DD));
}

Parabola::Parabola(const Point& focus,
                   const std::shared_ptr<Polygon>& poly,
                   SelectivePriorityQueue<CircleEvent>::iterator eventIt)
    : focus(focus),
      poly(poly),
      eventIt(eventIt)
{
}

CircleEvent::CircleEvent(const PointF& center,
                         double x,
                         ParaTree::iterator const& paraIt)
    : center(center),
      x(x),
      paraTreeIt(paraIt)
{
}

SweepLine::SweepLine()
    : beachTree(this->L)
{
}

SweepLine::SweepLine(std::shared_ptr<Voronoi> vmap)
    : beachTree(this->L)
{
    this->loadVmap(vmap);
}

void SweepLine::loadVmap(std::shared_ptr<Voronoi> vmap)
{
    this->vmap = vmap;
    beachTree.clear();
    siteEvent.clear();
    circleEvent.clear();
    for (const auto& poly_sptr : vmap->polygons) {
        poly_sptr->edges.clear();
        poly_sptr->unOrganize();
        this->addSite(poly_sptr);
    }
}

void SweepLine::addSite(const std::shared_ptr<Polygon>& poly)
{
    // prevent duplicate points
    auto it = siteEvent.lower_bound(poly->focus);
    if (it != siteEvent.end() && it->poly->focus == poly->focus)
        return;
    siteEvent.emplace(poly);
}

double SweepLine::nextEvent()
{
    L = LMAXVALUE;
    if (siteEvent.empty() && circleEvent.empty()) {
        return L;
    }
    if (siteEvent.size() &&
        (circleEvent.empty() || siteEvent.top().x < circleEvent.top().x)) {
        // site event
        L = siteEvent.top().x;
        beachAdd(siteEvent.top().poly);
        siteEvent.pop();
        return L;
    };
    // circle event
    CircleEvent event = circleEvent.top();
    circleEvent.pop();
    L = event.x;
    PointF eventPoint = event.center;

    /* our sweepline goes from left to right, perpendicular to x-axis,
     * thus from up to down, the order for parabolas is
     * pk   (above)
     * pj   (the one to be deleted from beachline)
     * pi   (beneath)
     */
    assertm(event.paraTreeIt != beachTree.begin() &&
                std::next(event.paraTreeIt) != beachTree.end(),
            "pj (the one to be deleted from beachline) must not be the first "
            "nor the last in beachline");
    Parabola& pk = *std::next(event.paraTreeIt);
    Parabola& pj = *event.paraTreeIt;
    Parabola& pi = *std::prev(event.paraTreeIt);

    // new edge for parabola above and beneath pj
    auto newEdge = std::make_shared<Edge>();
    auto newPoint = std::make_shared<Point>(eventPoint);
    newEdge->a = newPoint;
    pk.bottomEdge = newEdge;
    pk.poly->edges.push_back(newEdge);
    pi.topEdge = newEdge;
    pi.poly->edges.push_back(newEdge);

    // close the edges for current parabola
    if (!pj.bottomEdge->a) {
        pj.bottomEdge->a = newPoint;
    } else {
        pj.bottomEdge->b = newPoint;
    }
    if (!pj.topEdge->a) {
        pj.topEdge->a = newPoint;
    } else {
        pj.topEdge->b = newPoint;
    }

    auto prev = std::prev(event.paraTreeIt);
    auto next = std::next(event.paraTreeIt);
    if (event.paraTreeIt != beachTree.end()) {
        beachTree.erase(event.paraTreeIt);
    }
    // update neighbour's event
    checkCircleEvent(prev);
    checkCircleEvent(next);
    return L;
}

void SweepLine::beachAdd(const std::shared_ptr<Polygon>& poly)
{
    Parabola newPara(poly->focus, poly, circleEvent.end());
    if (beachTree.empty()) {
        beachTree.insert(beachTree.end(), newPara);
        return;
    }
    // paraIt is an iterator pointing to the parabola that's going to be cut in
    // half by newly added parabola
    auto paraTreeIt = beachTree.find(newPara);

    if (paraTreeIt->focus.x == poly->focus.x) {
        // special case, first two or more point on same x coordinate
        auto newEdge = std::make_shared<Edge>();
        // the new edge will be a horizontal line, whose y is in the middle of
        // two focus
        newEdge->a = std::make_shared<Point>(
            -LMAXVALUE, (paraTreeIt->focus.y + poly->focus.y) / 2);
        poly->edges.push_back(newEdge);
        paraTreeIt->poly->edges.push_back(newEdge);
        if (paraTreeIt->focus.y > poly->focus.y) {
            paraTreeIt->bottomEdge = newEdge;
            newPara.topEdge = newEdge;
            beachTree.insert(paraTreeIt, std::move(newPara));
        } else {
            paraTreeIt->topEdge = newEdge;
            newPara.bottomEdge = newEdge;
            beachTree.insert(std::next(paraTreeIt), std::move(newPara));
        }
        return;
    }

    Parabola dupPara(paraTreeIt->focus, paraTreeIt->poly, circleEvent.end());
    auto newEdge = std::make_shared<Edge>();

    poly->edges.push_back(newEdge);
    paraTreeIt->poly->edges.push_back(newEdge);

    dupPara.topEdge = paraTreeIt->topEdge;
    paraTreeIt->topEdge = newEdge;
    newPara.bottomEdge = newPara.topEdge = newEdge;
    dupPara.bottomEdge = newEdge;

    beachTree.insert(std::next(paraTreeIt), std::move(newPara));
    beachTree.insert(std::next(paraTreeIt, 2), std::move(dupPara));

    // remove deprecated circle event
    if (paraTreeIt->eventIt != circleEvent.end())
        circleEvent.erase(paraTreeIt->eventIt);
    paraTreeIt->eventIt = circleEvent.end();
    // now paraIt points to the new parabola
    ++paraTreeIt;
    // update neighbour's event
    checkCircleEvent(std::prev(paraTreeIt));
    checkCircleEvent(std::next(paraTreeIt));
}

void SweepLine::checkCircleEvent(beachTree_iterator const& paraTreeIt)
{
    Parabola& cur = *paraTreeIt;

    // remove deprecated event
    if (cur.eventIt != circleEvent.end())
        circleEvent.erase(cur.eventIt);
    cur.eventIt = circleEvent.end();

    // if cur is the first or last parabola in beachline, there's no way it can
    // generate an event
    if (paraTreeIt == beachTree.begin() ||
        std::next(paraTreeIt) == beachTree.end())
        return;

    Parabola& prev = *std::prev(paraTreeIt);
    Parabola& next = *std::next(paraTreeIt);
    // connect a line from `prev` to `cur` to `next`, it must be concave towards
    // where L is going for the circumcenter to be after `L` and be able to
    // close `cur` parabola
    if (cross(prev.focus, cur.focus, next.focus) >= 0)
        return;

    PointF center = findCircumcenter(prev.focus, cur.focus, next.focus);
    // center.x + radius of circumcircle = L's value when event happens
    double x = center.x + distance(cur.focus, center);
    auto eventIt = circleEvent.emplace(center, x, paraTreeIt);
    cur.eventIt = eventIt;
}

void SweepLine::finishEdges()
{
    if (beachTree.size() <= 1)
        return;
    // set L arbitrary large enough
    L = 2 * vmap->width + 2 * vmap->height;
    for (auto it = beachTree.begin(); it != std::prev(beachTree.end()); ++it) {
        PointF intersection =
            getIntersect(it->focus, std::next(it)->focus, this->L);
        auto newPoint = std::make_shared<Point>(intersection);
        if (!it->topEdge->a)
            it->topEdge->a = newPoint;
        else
            it->topEdge->b = newPoint;
    }
}

void SweepLine::performFortune()
{
    while (nextEvent() != LMAXVALUE)
        ;
    finishEdges();
}

double SweepLine::parabolaX(const Point& focus, double y)
{
    /**
     *  note parabola equation:
     *  (y-k)^2=4c(x-h), with directrix = h-c, focus = (h+c, k)
     *  now set focus as (h', k'), directrix = L
     *  we can get k=k', h=(h'+L)/2, c=(h'-L)/2
     */
    double k = focus.y;
    double h = (focus.x + L) / 2;
    double c = (focus.x - L) / 2;
    if (c >= 0) {
        // c >= 0 means `L`, our directrix, hasn't pass point `focus` yet
        return -LMAXVALUE;
    } else {
        return pow(y - k, 2) / (4 * c) + h;
    }
}

PointF getIntersect(const PointF& A, const PointF& B, double directrix)
{
    double ka = A.y;
    double ha = (directrix + A.x) / 2.0;
    double ca = -(directrix - A.x) / 2.0;
    double kb = B.y;
    double hb = (directrix + B.x) / 2.0;
    double cb = -(directrix - B.x) / 2.0;
    double a = cb - ca;
    double b = -2 * (cb * ka - ca * kb);
    double c = -(4 * ca * cb * (hb - ha) - cb * ka * ka + ca * kb * kb);
    double discriminant = b * b - 4 * a * c;
    if (a == 0) {
        // a == 0 means that A.x == B.x
        if (b == 0) {
            // a == 0 && b == 0 means that A.x == B.x && A.y == B.y
            return PointF(-SweepLine::LMAXVALUE, (A.y + B.y) / 2);
        }
        double y = -c / b;
        double x = pow(y - ka, 2) / (4 * ca) + ha;
        return PointF((float) x, (float) y);
    } else {
        double y = (-b - sqrt(discriminant)) / (2 * a);
        double x = pow(y - ka, 2) / (4 * ca) + ha;
        return PointF(x, y);
    }
}
