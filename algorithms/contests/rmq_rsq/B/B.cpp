#include <iostream>


template <typename T> // [l; r)
class Segment_tree {

public:

    Segment_tree(int n, int* a): tree(new T[4 * n + 1]) {
        build(1, 0, n, a);
    }

    ~Segment_tree() {
        delete[] tree;
    }

    void update(int v, int left, int right, int index, T delta) {

        tree[v] += delta;

        if (left == right - 1) {
            return;
        }

        int mid = (left + right) >> 1;

        if (index < mid) {
            update(2 * v, left, mid, index, delta);
        } else {
            update(2 * v + 1, mid, right, index, delta);
        }

    }

    T get_sum(int v, int left, int right, int req_l, int req_r) {

        if (req_l == left && req_r == right) {
            return tree[v];
        }

        int mid = (left + right) >> 1;
        int ans = 0;

        if (req_l < mid) {
            ans += get_sum(2 * v, left, mid, req_l, min(mid, req_r));
        }
        if (req_r > mid) {
            ans += get_sum(2 * v + 1, mid, right, max(req_l, mid), req_r);
        }

        return ans;

    }

    void print() {

        for (int i = 0; i < 13; ++i) {
            std::cout << i << ' ';
        }

        std::cout << '\n';

        for (int i = 0; i < 13; ++i) {
            std::cout << tree[i] << ' ';
        }

    }

private:

    T* tree;

    void build(int v, int left, int right, int *a) {

        if (left == right - 1) {
            tree[v] = left % 2 == 0 ? a[left] : -a[left];
            return;
        }

        int mid = (left + right) >> 1;

        build(2 * v, left, mid, a);
        build(2 * v + 1, mid, right, a);
        tree[v] = tree[2 * v] + tree[2 * v + 1];

    }

    int min(int a, int b) {
        return a < b ? a : b;
    }

    int max(int a, int b) {
        return a > b ? a : b;
    }

};


int main() {

    int n;
    scanf("%d", &n);

    int *a = new int[n];

    for (int i = 0; i < n; ++i) {
        scanf("%d", a + i);
    }

    Segment_tree<int> tree(n, a);

    int m;
    std::cin >> m;

    for (int i = 0; i < m; ++i) {

        int c, x ,y;
        std::cin >> c >> x >> y;

        if (c == 0) {
            int delta = y - a[x - 1];
            a[x - 1] = y;
            tree.update(1, 0, n, x - 1, x % 2 == 0 ? -delta : delta);
        } else {
           std::cout << (x % 2 == 0
                           ? -tree.get_sum(1, 0, n, x - 1, y)
                           : tree.get_sum(1, 0, n, x - 1, y)) << '\n';
        }

    }

}