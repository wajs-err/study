#include <iostream>
#include <vector>


void partition(std::vector<int>& a, int left, int right, int x, int& l, int& e) {

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


int quick_select(std::vector <int> &a, int left, int right, int k) {

    if (right - left <= 1) {
        return a[left];
    }

    int x = a[(left + right) / 2];
    int less, equal;

    partition(a, left, right, x, less, equal);

    if (k <= less) {
        return quick_select(a, left, left + less, k);
    }

    if (k <= less + equal) {
        return x;
    }

    return quick_select(a, left + less + equal, right, k - less - equal);

}


void quick_sort(std::vector <int> &a, int left, int right) {

    if (right - left <= 1) {
        return;
    }

    int l, e;
    int x = quick_select(a, left, right, (right - left + 1) / 2);

    partition(a, left, right, x, l, e);
    quick_sort(a, left, left + l);
    quick_sort(a, left + l + e, right);

}


int main()
{

    int n;
    std::cin >> n;

    std::vector <int> a(n);

    for (int &el : a) {
        std::cin >> el;
    }

    quick_sort(a, 0, n);

    for (int &el : a) {
        std::cout << el << ' ';
    }
    std::cout << '\n';

}