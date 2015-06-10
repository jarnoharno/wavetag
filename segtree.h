#ifndef SEGTREE_H
#define SEGTREE_H

#include <vector>
#include <algorithm>
#include <iterator>

int lg2(int n);

template <typename T>
struct MinMax
{
    T mn;
    T mx;
    MinMax(): mn(0), mx(-1) {}
    MinMax(const T& mn, const T& mx): mn(mn), mx(mx) {}
    MinMax(const T& x): mn(x), mx(x) {}
    bool nan() const
    {
        return mx < mn;
    }
    MinMax& merge(const MinMax<T> &b)
    {
        if (nan() && b.nan()) {
            return *this;
        }
        if (nan()) {
            *this = b;
            return *this;
        }
        if (b.nan()) {
            return *this;
        }
        mn = std::min(mn,b.mn);
        mx = std::max(mx,b.mx);
        return *this;
    }
};

template <typename T>
MinMax<T> merge(const MinMax<T>& a, const MinMax<T>& b)
{
    return MinMax<T>(a).merge(b);
}

template <typename T>
class SegTree
{
public:
    SegTree(): n(0), m(0) {}
    template <typename It>
    SegTree(It b, It e): n(e-b), m(lg2(n)), p(2*m)
    {
        auto o = p.begin()+m;
        while (b < e) {
            *o++ = T(*b++);
        }
        for (int i = m-1; i > 0; --i) {
            p[i] = merge(p[2*i],p[2*i+1]);
        }
    }

    T range(int l, int r) const
    {
        l += m;
        r += m - 1;
        T s;
        while (l <= r) {
            if (l%2 == 1) s.merge(p[l++]);
            if (r%2 == 0) s.merge(p[r--]);
            l /= 2;
            r /= 2;
        }
        return s;
    }

    int length() const { return n; }
    int empty() const { return n == 0; }
    const T& operator[](int i) { return p[m+i]; }

private:
    int n;
    int m;
    std::vector<T> p;
};

#endif // SEGTREE_H
