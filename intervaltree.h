#ifndef INTERVALTREE_H
#define INTERVALTREE_H

#include <set>

// left inclusive, right exclusive
template <typename T>
struct IntervalBound
{
    T x;
    bool right;
    IntervalBound(const T& x): x(x), right(true) {}
    IntervalBound(const T& x, bool r): x(x), right(r) {}
};

template <typename T>
bool operator<(const IntervalBound<T>& a, const IntervalBound<T>& b)
{
    return a.x < b.x || (a.x == b.x && !a.right && b.right);
}

template <typename T>
class IntervalTree
{
public:
    IntervalTree() {}

    bool inside(const T& x) const
    {
        auto next = s.upper_bound(IntervalBound<T>(x));
        return next != s.end() && next->right;
    }

    void add(const T& left, const T& right)
    {
        if (left >= right) return;
        IntervalBound<T> l(left, false);
        IntervalBound<T> r(right, true);
        auto u = s.lower_bound(r);
        s.erase(s.upper_bound(l),u);
        if (u == s.end() || !u->right) {
            u = s.emplace(std::move(r)).first;
        }
        if ((--u)->right) {
            s.emplace(std::move(l));
        }
    }

    void subtract(const T& left, const T& right)
    {
        if (left >= right) return;
        IntervalBound<T> l(left, false);
        IntervalBound<T> r(right, true);
        auto u = s.upper_bound(r);
        s.erase(s.lower_bound(l),u);
        if (u != s.end() && u->right) {
            r.right = false;
            u = s.emplace(std::move(r)).first;
        }
        --u;
        if (u != s.end() && !u->right) {
            l.right = true;
            s.emplace(std::move(l));
        }
    }

    int bounds() const
    {
        return s.size();
    }

    int intervals() const
    {
        return s.size()/2;
    }

private:
    std::set<IntervalBound<T>> s;
};

#endif // INTERVALTREE_H
