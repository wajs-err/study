#include <iostream>
#include <vector>


void partition(std::vector <int> &a, int left, int right, int x, int &l, int &e)
{

    int less = 0, equal = 0;
    std::vector <int> b;

    for (int i = left; i < right; ++i) {

        if (a[i] < x) {
            b.push_back(a[i]);
            ++less;
        }

    }

    for (int i = left; i < right; ++i) {

        if (a[i] == x) {
            b.push_back(a[i]);
            ++equal;
        }

    }

    for (int i = left; i < right; ++i) {

        if (a[i] > x) {
            b.push_back(a[i]);
        }

    }

    for (int i = left; i < right; ++i) {
        a[i] = b[i - left];
    }

    l = less; e = equal;

}


int quick_select(std::vector <int> &a, int left, int right, int k)
{

    if (right - left <= 1)
        return a[left];

    int x = a[ left + rand() % (right - left) ];
    int less, equal;

    partition(a, left, right, x, less, equal);

    if (k <= less)
        return quick_select(a, left, left + less, k);

    if (k <= less + equal)
        return x;

    return quick_select(a, left + less + equal, right, k - less - equal);

}


int main()
{

    int n, k;
    std::cin >> n >> k;

    std::vector <int> a(n);

    for (int &el : a) {
        std::cin >> el;
    }

    std::cout << quick_select(a, 0, n, k + 1) << '\n';

}