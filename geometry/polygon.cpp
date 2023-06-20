#include "polygon.h"

Polygon::Polygon()
    : organized(false)
{
}

Polygon::Polygon(const Polygon& old)
{
    for (const auto& edge : old.edges) {
        this->edges.push_back(edge);
    }
    this->focus = new Point(old.focus);
    // if old one is organized, since we are copying edges in order,
    // new one must also been organized, vice versa.
    this->organized = old.organized;
}

Polygon::Polygon(Point f)
{
    this->focus = std::move(f);
}

Polygon::Polygon(double focusx, double focusy)
    : focus(new Point((int) focusx, (int) focusy))
{
}

bool Polygon::contains(const Point& other)
{
    if (!organized)
        organize();
    if (!this->isComplete())
        return false;
    for (const auto& edge : edges) {
        if (cross(edge.a, edge.b, other) < 0)
            return false;
    }
    return true;
}

bool Polygon::contains(const int x, const int y)
{
    Point bar(x, y);
    return contains(bar);
}

void Polygon::organize()
{
    std::vector<double> edgeDegrees(edges.size());
    for (size_t i = 0; i < edges.size(); i++) {
        // add average degree of edge to array for sorting uses later
        double ta = atan2(edges[i].a.y - focus.y, edges[i].a.x - focus.x);
        double tb = atan2(edges[i].b.y - focus.y, edges[i].b.x - focus.x);
        double degree = (ta + tb) / 2;
        if (abs(ta - tb) > PI) {
            degree += PI;
            for (; degree > PI; degree -= 2 * PI)
                ;
            for (; degree < -PI; degree += 2 * PI)
                ;
        }
        edgeDegrees[i] = degree;

        // sort a, b
        if (degree < -PI / 2 || degree > PI / 2) {
            // angle of edge is behind focus
            if (tb > 0 && ta < 0)
                ta += 2 * PI;
            else if (ta > 0 && tb < 0)
                tb += 2 * PI;
        }
        if (ta >= tb) {
            std::swap(edges[i].a, edges[i].b);
        }
    }
    pairsort(edgeDegrees.begin(), edgeDegrees.end(), edges.begin());
    organized = true;
}

void Polygon::unOrganize()
{
    organized = false;
}

bool Polygon::isComplete()
{
    std::set<Point> s;
    for (const Edge& it : edges) {
        // incomplete edge
        if (!it.isComplete)
            return false;
        if (s.count(it.a)) {
            s.erase(it.a);
        } else {
            s.insert(it.a);
        }
        if (s.count(it.b)) {
            s.erase(it.b);
        } else {
            s.insert(it.b);
        }
    }
    return s.size() == 0;
}

template <typename T>
struct is_comparable {
    template <typename U>
    using comparability = decltype(std::declval<U>() < std::declval<U>());
    template <typename U, class = std::void_t<>>
    struct test : std::false_type {
    };
    template <typename U>
    struct test<U, std::void_t<comparability<U>>> : std::true_type {
    };

    static constexpr bool value = test<T>::value;
};

template <typename It1, typename It2>
void pairsort(It1 first, It1 last, It2 pFirst)
{
    using T1 = typename std::iterator_traits<It1>::value_type;
    using T2 = typename std::iterator_traits<It2>::value_type;
    std::vector<std::pair<T1, T2>> pairArr;
    pairArr.reserve(std::distance(first, last));

    std::sort(pairArr.begin(), pairArr.end(), [](const auto& a, const auto& b) {
        if constexpr (is_comparable<decltype(a.second)>::value) {
            if (a.first == b.first)
                return a.second < b.second;
        }
        return a.first < b.first;
    });

    for (auto it = first, pit = pFirst; it != last; ++it, ++pit) {
        pairArr.push_back({std::move(*it), std::move(*pit)});
    }

    for (auto& val : pairArr) {
        *(first++) = std::move(val.first);
        *(pFirst++) = std::move(val.second);
    }
}

template <typename It1, typename It2, class Comp>
void pairsort(It1 first, It1 last, It2 pFirst, Comp comp)
{
    using T1 = typename std::iterator_traits<It1>::value_type;
    using T2 = typename std::iterator_traits<It2>::value_type;
    std::vector<std::pair<T1, T2>> pairArr;
    pairArr.reserve(std::distance(first, last));
    for (auto it = first, pit = pFirst; it != last; ++it, ++pit) {
        pairArr.push_back({std::move(*it), std::move(*pit)});
    }

    std::sort(pairArr.begin(), pairArr.end(), comp);

    for (auto& val : pairArr) {
        *(first++) = std::move(val.first);
        *(pFirst++) = std::move(val.second);
    }
}
