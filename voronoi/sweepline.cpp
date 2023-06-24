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
                         std::list<Parabola>::iterator const& paraIt)
    : center(center),
      x(x),
      paraIt(paraIt)
{
}

SweepLine::SweepLine(std::shared_ptr<Voronoi> vmap)
{
    this->loadVmap(vmap);
}

void SweepLine::loadVmap(std::shared_ptr<Voronoi> vmap)
{
    this->vmap = vmap;
    beachParas.clear();
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
    SiteEvent newEvent(poly);
    // prevent duplicate points
    auto it = siteEvent.lower_bound(newEvent);
    if (it != siteEvent.end() && it->poly->focus == poly->focus)
        return;
    siteEvent.push(std::move(newEvent));
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
    assertm(event.paraIt != beachParas.begin() &&
                std::next(event.paraIt) != beachParas.end(),
            "pj (the one to be deleted from beachline) must not be the first "
            "nor the last in beachline");
    Parabola& pk = *std::next(event.paraIt);
    Parabola& pj = *event.paraIt;
    Parabola& pi = *std::prev(event.paraIt);

    // new edge for above and beneath parabola
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

    auto prev = std::prev(event.paraIt);
    auto next = std::next(event.paraIt);
    if (event.paraIt != beachParas.end())
        beachParas.erase(event.paraIt);
    // update neighbour's event
    dealCircleEvent(prev);
    dealCircleEvent(next);
    return L;
}

void SweepLine::beachAdd(const std::shared_ptr<Polygon>& poly)
{
    Parabola newPara(poly->focus, poly, circleEvent.end());
    if (beachParas.empty()) {
        beachParas.push_back(newPara);
        return;
    }
    // paraIt is an iterator pointing to the parabola that's going to be cut in
    // half by newly added parabola
    auto paraIt = beachParas.end();
    for (auto it = beachParas.begin(); it != beachParas.end(); ++it) {
        double intxn1 = -LMAXVALUE, intxn2 = LMAXVALUE;
        if (it != beachParas.begin())
            intxn1 = getIntersect(std::prev(it)->focus, it->focus).y;
        if (std::next(it) != beachParas.end())
            intxn2 = getIntersect(it->focus, std::next(it)->focus).y;
        if (intxn1 <= poly->focus.y && poly->focus.y < intxn2) {
            paraIt = it;
            break;
        }
    }

    if (paraIt->focus.x == poly->focus.x) {
        // special case, first two or more point on same x coordinate
        auto newEdge = std::make_shared<Edge>();
        // the new edge will be a horizontal line, whose y is in the middle of
        // two focus
        newEdge->a = std::make_shared<Point>(
            -LMAXVALUE, (paraIt->focus.y + poly->focus.y) / 2);
        poly->edges.push_back(newEdge);
        paraIt->poly->edges.push_back(newEdge);
        if (paraIt->focus.y > poly->focus.y) {
            paraIt->bottomEdge = newEdge;
            newPara.topEdge = newEdge;
            beachParas.insert(paraIt, std::move(newPara));
        } else {
            paraIt->topEdge = newEdge;
            newPara.bottomEdge = newEdge;
            beachParas.insert(std::next(paraIt), std::move(newPara));
        }
        return;
    } else if (beachParas.size() == 2) {
        // special case, on third point insertion, first two points are on the
        // same x
        double intersection =
            getIntersect(beachParas.front().focus, beachParas.back().focus).y;
        paraIt = beachParas.begin();
        if (poly->focus.y >= intersection) {
            ++paraIt;
        }
    }

    Parabola dupPara(paraIt->focus, paraIt->poly, circleEvent.end());
    auto newEdge = std::make_shared<Edge>();

    poly->edges.push_back(newEdge);
    paraIt->poly->edges.push_back(newEdge);

    dupPara.topEdge = paraIt->topEdge;
    paraIt->topEdge = newEdge;
    newPara.bottomEdge = newPara.topEdge = newEdge;
    dupPara.bottomEdge = newEdge;

    beachParas.insert(std::next(paraIt), std::move(newPara));
    beachParas.insert(std::next(paraIt, 2), std::move(dupPara));

    // remove deprecated circle event
    if (paraIt->eventIt != circleEvent.end())
        circleEvent.erase(paraIt->eventIt);
    paraIt->eventIt = circleEvent.end();
    // now paraIt points to the new parabola
    ++paraIt;
    // update neighbour's event
    dealCircleEvent(std::prev(paraIt));
    dealCircleEvent(std::next(paraIt));
}

void SweepLine::dealCircleEvent(std::list<Parabola>::iterator const& paraIt)
{
    Parabola& cur = *paraIt;

    // remove deprecated event
    if (cur.eventIt != circleEvent.end())
        circleEvent.erase(cur.eventIt);
    cur.eventIt = circleEvent.end();

    // if cur is the first or last parabola in beachline, there's no way it can
    // generate an event
    if (paraIt == beachParas.begin() || std::next(paraIt) == beachParas.end())
        return;

    Parabola& prev = *std::prev(paraIt);
    Parabola& next = *std::next(paraIt);
    // connect a line from `prev` to `cur` to `next`, it must be concave towards
    // where L is going for the circumcenter to be after `L` and be able to
    // close `cur` parabola
    if (cross(prev.focus, cur.focus, next.focus) >= 0)
        return;

    PointF center = findCircumcenter(prev.focus, cur.focus, next.focus);
    // center.x + radius of circumcircle = L's value when event happens
    double x = center.x + distance(cur.focus, center);
    CircleEvent newEvent(center, x, paraIt);
    auto eventIt = circleEvent.push(newEvent);
    cur.eventIt = eventIt;
}

void SweepLine::finishEdges()
{
    // set L arbitrary large enough
    L = 2 * vmap->width + 2 * vmap->height;
    for (auto it = beachParas.begin(); it != std::prev(beachParas.end());
         ++it) {
        PointF intersection = getIntersect(it->focus, std::next(it)->focus);
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

PointF SweepLine::getIntersect(const PointF& A, const PointF& B)
{
    /**
     *  note parabola equation:
     *  (y-k)^2=4c(x-h), with directrix = h-c, focus = (h+c, k)
     *  now set focus as (h', k'), directrix = L
     *  we can get k=k', h=(h'+L)/2, c=(h'-L)/2
     *
     *  to solve intersection, we rearrange parabola equation to:
     *  ((y-k_1)^2)/(4c_1) + h_1 = x = ((y-k_2)^2)/(4c_2) + h_2
     *
     *  rearrange again gives us
     *  (c_2 - c_1)y^2 + (-2k_1c_2 + 2k_2c_1)y + \
     *  (k_1^2c_2-k_2^2c_1 + 4c_1c_2(h_1 - h_2)) = 0
     *
     *  substitude in quadratic formula: ay^2+by+c = 0
     *  a = c_2-c_1
     *  b = -2k_1c_2 + 2k_2c_1
     *  c = k_1^2c_2-k_2^2c_1 + 4c_1c_2(h_1 - h_2)
     */
    double ka = A.y;
    double ha = (L + A.x) / 2.0;
    double ca = -(L - A.x) / 2.0;
    double kb = B.y;
    double hb = (L + B.x) / 2.0;
    double cb = -(L - B.x) / 2.0;
    double a = cb - ca;
    double b = -2 * (cb * ka - ca * kb);
    double c = -(4 * ca * cb * (hb - ha) - cb * ka * ka + ca * kb * kb);
    double discriminant = b * b - 4 * a * c;
    if (a == 0) {
        // a == 0 means that A.x == B.x
        if (b == 0) {
            // a == 0 && b == 0 means that A.x == B.x && A.y == B.y
            return PointF(-LMAXVALUE, (A.y + B.y) / 2);
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
