#ifndef SELECTIVEPRIORITYQUEUE_H
#define SELECTIVEPRIORITYQUEUE_H

#include <set>

template <class Key, class Compare = std::less<Key>>
class SelectivePriorityQueue : public std::multiset<Key, Compare>
{
public:
    // Inheriting constructors
    using std::multiset<Key, Compare>::multiset;

    using typename std::multiset<Key, Compare>::const_reference;
    using typename std::multiset<Key, Compare>::iterator;

    template <class T>
    iterator push(T&& value)
    {
        return this->insert(std::forward<T>(value));
    }
    template <class T>
    iterator push(const_reference pos, T&& value)
    {
        return this->insert(pos, std::forward<T>(value));
    }
    const_reference top() { return *prev(this->end()); }
    void pop() { this->erase(prev(this->end())); }
};

#endif  // SELECTIVEPRIORITYQUEUE_H
