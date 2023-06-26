#ifndef SWEEPLINE_H
#define SWEEPLINE_H

#include <cassert>
#include <iterator>
#include <list>

#include "data_structure/selectivepriorityqueue.h"
#include "geometry/polygon.h"
#include "voronoi.h"

class CircleEvent;

/**
 * @brief get intersection of two parabola given focus point A and B
 * with directrix
 * @param A focus point of parabola
 * @param B focus point of parabola
 * @return intersection point of two parabola
 */
PointF getIntersect(const PointF& A, const PointF& B, double directrix);

class Parabola
{
public:
	Parabola(const Point& focus,
			 const std::shared_ptr<Polygon>& poly,
			 SelectivePriorityQueue<CircleEvent>::iterator eventIt);
	~Parabola() = default;

	const PointF focus;

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

class ParaTree
{
private:
	std::list<Parabola> beachParas;
	using Container = decltype(beachParas);
	struct IterCompare {
		using is_transparent = void;
		using key_type = Parabola;
		using iterator = Container::const_iterator;

		IterCompare(const double& L, const Container& c)
			: c(c),
			  L(L)
		{
		}

		const Container& c;
		const double& L;
		bool operator()(const iterator&, const iterator&) const
		{
			return false;
		}
		bool operator()(const iterator& it, const key_type& key) const
		{
			if (std::next(it) == c.end())
				return false;
			double intxn2 = getIntersect(it->focus, std::next(it)->focus, L).y;
			return intxn2 < key.focus.y;
		}
		bool operator()(const key_type& key, const iterator& it) const
		{
			if (it == c.begin())
				return false;
			double intxn1 = getIntersect(std::prev(it)->focus, it->focus, L).y;
			return key.focus.y < intxn1;
		}
	};
	std::multiset<std::list<Parabola>::iterator, IterCompare> beachTree;

public:
	class iterator
	{
		decltype(beachTree)::iterator it;
		friend class ParaTree;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = Container::difference_type;
		using pointer = Container::pointer;
		using reference = Container::reference;
		using value_type = Container::value_type;

		iterator(decltype(beachTree)::iterator _it)
			: it(_it)
		{
		}
		iterator& operator++()
		{
			++it;
			return *this;
		}
		iterator& operator--()
		{
			--it;
			return *this;
		}
		iterator operator++(int)
		{
			iterator rtv = *this;
			++(*this);
			return rtv;
		}
		iterator operator--(int)
		{
			iterator rtv = *this;
			--(*this);
			return rtv;
		}
		bool operator==(const iterator& other) const { return it == other.it; }
		bool operator!=(const iterator& other) const
		{
			return !(*this == other);
		}
		reference operator*() const { return **it; }
		pointer operator->() const { return &(**it); }
	};
	using key_type = Parabola;
	using reference = Parabola&;
	using size_type = decltype(beachTree)::size_type;
	ParaTree(const double& L)
		: beachTree(IterCompare(L, beachParas))
	{
	}
	void clear()
	{
		beachParas.clear();
		beachTree.clear();
	}
	iterator begin() { return iterator(beachTree.begin()); }
	iterator end() { return iterator(beachTree.end()); }
	iterator erase(iterator pos)
	{
		beachParas.erase(*pos.it);
		return beachTree.erase(pos.it);
	}
	bool empty() { return beachTree.empty(); }
	template <class T>
	iterator insert(iterator pos, T&& value)
	{
		auto beachParasPos = (pos == this->end() ? beachParas.end() : *pos.it);
		auto it = beachParas.insert(beachParasPos, std::forward<T>(value));
		return iterator(beachTree.insert(pos.it, it));
	}
	size_type size() { return beachTree.size(); }
	iterator find(const key_type& key) { return iterator(beachTree.find(key)); }
	reference front() { return beachParas.front(); }
	reference back() { return beachParas.back(); }
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
	friend operator>(const PointF& lhs, const SiteEvent& rhs)
	{
		return std::tie(lhs.x, lhs.y) > std::tie(rhs.x, rhs.y);
	}
	friend operator>(const SiteEvent& lhs, const PointF& rhs)
	{
		return std::tie(lhs.x, lhs.y) > std::tie(rhs.x, rhs.y);
	}
};

class CircleEvent
{
public:
	CircleEvent(const PointF& center,
				double x,
				ParaTree::iterator const& paraIt);

	PointF center;  // center of the circumcenter
	double x;       // sweepline position when event happen

	/**
	 * @brief parabola associated with this event
	 * the one that'll be deleted from beachLine
	 */
	ParaTree::iterator const paraTreeIt;

	bool operator>(const CircleEvent& rhs) const { return x > rhs.x; }
};

class SweepLine
{
public:
	SweepLine();
	SweepLine(std::shared_ptr<Voronoi> vmap);

	// sweep line position
	double L;
	// parabolas who made up the beach line
	ParaTree beachTree;

public:
	using beachTree_iterator = ParaTree::iterator;

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
	void checkCircleEvent(beachTree_iterator const& paraTreeIt);

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

public:
	static constexpr double LMAXVALUE = std::numeric_limits<double>::max();
	static constexpr float MAXVALUE = std::numeric_limits<float>::max();
};

#endif  // SWEEPLINE_H
