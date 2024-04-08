#include <iostream>
#include <vector>


template <typename T>
class binary_heap {

public:

    T* arr = new T[1001];

    binary_heap() {}

    ~binary_heap() {
        delete [] arr;
    }

    void insert(T val) {
        curr++;
        arr[curr] = val;
        sift_up(curr);
    }

    T get_max() {
        return arr[1];
    }

    void extract_max() {
        swap(1, curr);
        curr--;
        sift_down(1);
    }

    int size() {
        return curr;
    }

private:

    int curr = 0;

    void swap(int x, int y) {

        T tmp = arr[x];
        arr[x] = arr[y];
        arr[y] = tmp;

    }

    void sift_up(int v) {

        while (v != 1) {

            if (arr[v] > arr[v / 2]) {
                swap(v, v / 2);
                v /= 2;
            } else {
                break;
            }

        }

    }

    void sift_down(int v) {

        while (2 * v <= curr) {

            int u = 2 * v;

            if (2 * v  + 1 <= curr && arr[2 * v + 1] > arr[2 * v]) {
                u = 2 * v + 1;
            }

            if (arr[u] > arr[v]) {
                swap(u, v);
                v = u;
            } else {
                break;
            }

        }

    }

};


void partition(std::vector<long long>& a, int left, int right, long long x, int& l, int& e) {

    int less = 0, equal = 0;
    std::vector <long long> b;

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


int quick_select(std::vector<long long>& a, int left, int right, int k)
{

    if (right - left <= 1)
        return a[left];

    int x = a[ (left + right) / 2 ];
    int less, equal;

    partition(a, left, right, x, less, equal);

    if (k <= less)
        return quick_select(a, left, left + less, k);

    if (k <= less + equal)
        return x;

    return quick_select(a, left + less + equal, right, k - less - equal);

}


void quick_sort(std::vector<long long>& a, int left, int right) {

    if (right - left <= 1)
        return;

    int l, e;
    int x = quick_select(a, left, right, (right - left + 1) / 2);

    partition(a, left, right, x, l, e);
    quick_sort(a, left, left + l);
    quick_sort(a, left + l + e, right);

}


int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    binary_heap <long long> h;

    int n, k;
    std::cin >> n >> k;

    long long a, x, y;
    std::cin >> a >> x >> y;

    for (int i = 1; i <= n; ++i) {

        long long _a = (x * a + y) % (1 << 30);

        if (h.size() < k) {
            h.insert(_a);
        } else if (_a < h.get_max()) {
            h.extract_max();
            h.insert(_a);
        }

        a = _a;

    }

    std::vector <long long> b;

    for (int i = 1; i <= k; ++i) {
        b.push_back(h.arr[i]);
    }

    quick_sort(b, 0, k);

    for (long long &el : b) {
        std::cout << el << ' ';
    }
    std::cout << '\n';

}