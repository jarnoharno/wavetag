#ifndef SEGTREE_H
#define SEGTREE_H

#include <vector>
#include <algorithm>
#include <iterator>

int lg2(int n);

template <typename T>
struct MinMaxData
{
    T mn;
    T mx;
    MinMaxData(): mn(0), mx(-1) {}
    MinMaxData(const T& x): mn(x), mx(x) {}
    MinMaxData(const T& mn, const T& mx): mn(mn), mx(mx) {}
    bool nan() const
    {
        return mx < mn;
    }
};

template <typename T>
struct MinMax
{
    typedef MinMaxData<T> Value;
    static Value merge(const Value& a, const Value& b)
    {
        if (a.nan() && b.nan()) return Value();
        if (a.nan()) return b;
        if (b.nan()) return a;
        return Value(std::min(a.mn,b.mn),std::max(a.mx,b.mx));
    }
};

template <typename T>
struct Or
{
    typedef T Value;
    static Value merge(const Value& a, const Value& b) {
        return a | b;
    }
};

template <typename T>
class SegTree
{
public:
    typedef typename T::Value Value;
    SegTree(): n(0), m(0) {}
    SegTree(int n): n(n), m(lg2(n)), p(2*m) {}
    template <typename It>
    SegTree(It b, It e): n(e-b), m(lg2(n)), p(2*m)
    {
        auto o = p.begin()+m;
        while (b < e) {
            *o++ = Value(*b++);
        }
        for (int i = m-1; i > 0; --i) {
            p[i] = T::merge(p[2*i],p[2*i+1]);
        }
    }

    Value range(int l, int r) const
    {
        l += m;
        r += m - 1;
        Value s;
        while (l <= r) {
            if (l%2 == 1) s = T::merge(s,p[l++]);
            if (r%2 == 0) s = T::merge(s,p[r--]);
            l /= 2;
            r /= 2;
        }
        return s;
    }

    int length() const { return n; }
    int empty() const { return n == 0; }
    const Value& operator[](int i) { return p[m+i]; }

private:
    int n;
    int m;
    std::vector<Value> p;
};

#endif // SEGTREE_H
