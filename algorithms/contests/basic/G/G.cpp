#include <iostream>


bool check(long long x, int p, int q, int *a, size_t n, int mx)
{

    long long s = 0;

    if (p == q)
        return (mx + p - 1) / p <= x;

    for (size_t i = 0; i < n; ++i)
        if (a[i] - x * q >= 0)
            s += (a[i] - x * q + p - q - 1) / (p - q);

    return s <= x;

}


int main()
{

    size_t n;
    int p, q;
    std::cin >> n >> p >> q;

    int *a = new int [n];

    int mx = 0;

    for (size_t i = 0; i < n; ++i) {
        std::cin >> a[i];
        mx = std::max(a[i], mx);
    }

    long long l = 1, r = (mx + q - 1) / q;

    while (r - l > 1) {

        long long mid = (l + r) / 2;

        (check(mid, p, q, a, n, mx) ? r : l) = mid;

    }

    std::cout << r;

}