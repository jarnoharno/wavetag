#ifndef INTERVALTREE_H
#define INTERVALTREE_H

#include <set>
#include <iterator>
#include <memory>
#include <tuple>

// left inclusive, right exclusive
template <typename T>
struct IntervalBound
{
    T x;
    bool right;
    IntervalBound(T x): x(x), right(true) {}
    IntervalBound(T x, bool r): x(x), right(r) {}
};

template <typename T>
bool operator<(const IntervalBound<T>& a, const IntervalBound<T>& b)
{
    return a.x < b.x || (a.x == b.x && !a.right && b.right);
}

template <typename T>
struct Interval
{
    T left;
    T right;
    Interval(T left, T right): left(left), right(right) {}
};

// iterator returning proxy Intervals
template <typename T>
class IntervalIterator
{
public:
    typedef typename std::set<IntervalBound<T>>::const_iterator Iterator;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef Interval<T> value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type reference;

    IntervalIterator() {}
    IntervalIterator(const Iterator& it): it(it) {}

    reference operator*()
    {
        auto j = it;
        return value_type(it->x, (++j)->x);
    }

    std::unique_ptr<value_type> operator->()
    {
        auto j = it;
        return std::make_unique<value_type>(it->x, (++j)->x);
    }

    IntervalIterator& operator++()
    {
        ++++it;
        return *this;
    }

    IntervalIterator operator++(int)
    {
        auto j = *this;
        operator++();
        return j;
    }

    IntervalIterator& operator--()
    {
        ----it;
        return *this;
    }

    IntervalIterator& operator--(int)
    {
        auto j = *this;
        operator--();
        return j;
    }

    bool operator==(const IntervalIterator<T>& b) const
    {
        return it == b.it;
    }

    bool operator!=(const IntervalIterator<T>& b) const
    {
        return it != b.it;
    }

private:
    Iterator it;
};

template <typename T>
class IntervalRange
{
public:
    IntervalRange() {}
    IntervalRange(IntervalIterator<T> b, IntervalIterator<T> e): b(b), e(e) {}
    IntervalIterator<T> begin() { return b; }
    IntervalIterator<T> end() { return e; }
private:
    IntervalIterator<T> b;
    IntervalIterator<T> e;
};

template <typename T>
class IntervalTree
{
public:
    IntervalTree() {}

    bool inside(T x) const
    {
        auto next = s.upper_bound(IntervalBound<T>(x));
        return next != s.end() && next->right;
    }

    void add(T left, T right)
    {
        if (left == right) return;
        if (left > right) std::swap(left, right);
        IntervalBound<T> l(left, false);
        IntervalBound<T> r(right, true);
        auto u = s.lower_bound(r);
        s.erase(s.upper_bound(l),u);
        if (u == s.end() || !u->right) {
            u = s.emplace(std::move(r)).first;
        }
        if (u == s.begin() || (--u)->right) {
            s.emplace(std::move(l));
        }
    }

    void subtract(T left, T right)
    {
        if (left == right) return;
        if (left > right) std::swap(left, right);
        IntervalBound<T> l(left, false);
        IntervalBound<T> r(right, true);
        auto u = s.upper_bound(r);
        s.erase(s.lower_bound(l),u);
        if (u != s.end() && u->right) {
            r.right = false;
            u = s.emplace(std::move(r)).first;
        }
        if (u != s.begin() && !(--u)->right) {
            l.right = true;
            s.emplace(std::move(l));
        }
    }

    IntervalRange<T> overlapping(T left, T right)
    {
        if (left == right) return IntervalRange<T>(s.end(),s.end());
        if (left > right) std::swap(left, right);
        IntervalBound<T> l(left, true);
        IntervalBound<T> r(right, false);
        auto i = s.upper_bound(l);
        if (i->right) --i;
        auto j = s.lower_bound(r);
        if (j->right) ++j;
        return IntervalRange<T>(i,j);
    }

    int bounds() const
    {
        return s.size();
    }

    int intervals() const
    {
        return s.size()/2;
    }

    IntervalIterator<T> begin() const
    {
        return IntervalIterator<T>(s.begin());
    }
    IntervalIterator<T> end() const
    {
        return IntervalIterator<T>(s.end());
    }

private:
    std::set<IntervalBound<T>> s;
};

#endif // INTERVALTREE_H
