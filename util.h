#ifndef UTIL_H
#define UTIL_H

template <typename T>
T clamp(T x, T a, T b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

#endif // UTIL_H

