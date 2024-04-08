#include <fstream>


// [l, r)


long long merge(int *a, size_t l, size_t mid, size_t r) // merges [l, mid) and [mid, r) in a
{

    int *b = new int [mid - l];
    int *c = new int [r - mid];

    for (size_t i = 0; i < mid - l; ++i)
        b[i] = a[l + i];

    for (size_t i = 0; i < r - mid; ++i)
        c[i] = a[mid + i];

    size_t i = 0, j = 0;
    long long prev = 0, inv = 0;

    while (i < mid - l && j < r - mid)
        if (b[i] <= c[j]) {
            a[l + i + j] = b[i];
            ++i;
            inv += prev;
        } else {
            a[l + i + j] = c[j];
            ++j; ++prev;
        }

    while (i < mid - l) {
        a[l + i + j] = b[i];
        ++i;
        inv += prev;
    }

    while (j < r - mid) {
        a[l + i + j] = c[j];
        ++j; ++prev;
    }

    delete [] b;
    delete [] c;

    return inv;

}


long long merge_sort(int *a, size_t l, size_t r)
{

    if (r - l <= 1)
        return 0;

    size_t mid = (l + r) / 2;

    long long s = 0;

    s += merge_sort(a, l, mid);
    s += merge_sort(a, mid, r);

    s += merge(a, l, mid, r);

    return s;

}


int main()
{

    std::ifstream in("inverse.in");

    size_t N;
    in >> N;

    int *a = new int [N];

    for (size_t i = 0; i < N; ++i)
        in >> a[i];

    std::ofstream out("inverse.out");
    out << merge_sort(a, 0, N);

    delete [] a;

}