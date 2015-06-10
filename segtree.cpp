#include "segtree.h"

int lg2(int n)
{
    int m = 1;
    while (m < n) m *= 2;
    return m;
}

