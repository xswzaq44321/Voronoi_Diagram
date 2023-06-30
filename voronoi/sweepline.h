#ifndef SWEEPLINE_H
#define SWEEPLINE_H

#include <cassert>
#include <list>

#include "data_structure/selectivepriorityqueue.h"
#include "geometry/polygon.h"
#include "voronoi.h"

class CircleEvent;

class Parabola
{
public:
	Parabola(const Point& focus,
			 const std::shared_ptr<Polygon>& poly,
			 SelectivePriorityQueue<CircleEvent>::iterator eventIt);
	~Parabola() = default;

	PointF focus;

	/**
	 * @brief records which polygon does this parabola referring to
	 */
	std::shared_ptr<Polygon> poly;
	std::shared_ptr<Edge> bottomEdge, topEdge;

	/**
	 * @brief event iterator to event related to this parabola.
	 * use for efficient deletion purpose when the event gets invalidated
	 */
	SelectivePriorityQueue<CircleEvent>::iterator eventIt;
};

class SiteEvent
{
public:
	SiteEvent(const std::shared_ptr<Polygon>& poly)
		: x(poly->focus.x),
		  y(poly->focus.y),
		  poly(poly)
	{
	}

	double x;  // sweepline position when event happen
	double y;  // site's y coordinate, for compare equal purpose
	/**
	 * @brief polygon associated with this event
	 */
	std::shared_ptr<Polygon> poly;

	bool operator>(const SiteEvent& rhs) const
	{
		return std::tie(x, y) > std::tie(rhs.x, rhs.y);
	}
	friend bool operator>(const PointF& lhs, const SiteEvent& rhs)
	{
		return std::tie(lhs.x, lhs.y) > std::tie(rhs.x, rhs.y);
	}
	friend bool operator>(const SiteEvent& lhs, const PointF& rhs)
	{
		return std::tie(lhs.x, lhs.y) > std::tie(rhs.x, rhs.y);
	}
};

class CircleEvent
{
public:
	CircleEvent(const PointF& center,
				double x,
				std::list<Parabola>::iterator const& paraIt);

	PointF center;  // center of the circumcenter
	double x;       // sweepline position when event happen

	/**
	 * @brief parabola associated with this event
	 * the one that'll be deleted from beachLine
	 */
	std::list<Parabola>::iterator const paraIt;

	bool operator>(const CircleEvent& rhs) const { return x > rhs.x; }
};

class SweepLine
{
public:
	SweepLine() = default;
	SweepLine(std::shared_ptr<Voronoi> vmap);

	// sweep line position
	double L;

	// parabolas who made up the beach line
	std::list<Parabola> beachParas;

	// priority queues of events, min heap
	SelectivePriorityQueue<SiteEvent, std::greater<>> siteEvent;
	SelectivePriorityQueue<CircleEvent, std::greater<>> circleEvent;

	// voronoi map who stores important informations such as polygons
	std::shared_ptr<Voronoi> vmap;

	/**
	 * @brief set vmap and load it's content for preparation
	 * @param vmap shared_ptr to Voronoi
	 */
	void loadVmap(std::shared_ptr<Voronoi> vmap);
	/**
	 * @brief add site event
	 * site event is represented as polygon
	 * @param pol site event wrap up as polygon
	 */
	void addSite(const std::shared_ptr<Polygon>& poly);

	/**
	 * @brief process next event and return directrix value
	 * @return directrix value
	 */
	double nextEvent();

	/**
	 * @brief add parabola to beachline
	 * @param poly polygon related to to-be added parabola
	 */
	void beachAdd(const std::shared_ptr<Polygon>& poly);

	/**
	 * @brief handles newly added circle event
	 * also maintain existed circle event and remove deprecated events
	 * @param event new circle event to add
	 */
	void checkCircleEvent(std::list<Parabola>::iterator const& paraIt);

	/**
	 * @brief finish open edges
	 * when all event are handled, there are still some open edges, and we need
	 * to find the edges' other vertex, we can do so by setting directrix `L` to
	 * a very large value and set those to intersections of parabolas
	 */
	void finishEdges();

	/**
	 * @brief perform fortune's algorithm, and finish edges
	 */
	void performFortune();

	/**
	 * @brief returns parabola's x value given y
	 * with directrix using member variable `L`
	 */
	double parabolaX(const Point& focus, double y);

	/**
	 * @brief get intersection of two parabola given focus point A and B
	 * with directrix by using member variable `L`
	 * @param A focus point of parabola
	 * @param B focus point of parabola
	 * @return intersection point of two parabola
	 */
	PointF getIntersect(const PointF& A, const PointF& B);
	PointF getIntersect(const Point& A, const Point& B);

public:
	const double LMAXVALUE = std::numeric_limits<double>::max();
	const float MAXVALUE = std::numeric_limits<float>::max();
};

#endif  // SWEEPLINE_H
