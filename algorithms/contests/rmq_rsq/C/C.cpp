#include <iostream>


template <typename T> // [l; r)
class Segment_tree {

public:

    Segment_tree(int n, T* a): tree(new T[4 * n + 1]) {
        build(1, 1, n, a);
    }

    ~Segment_tree() {
        delete[] tree;
    }

    void update(int v, int left, int right, int index, T val) {

        if (left == right - 1) {
            tree[v] = val;
            return;
        }

        int mid = (left + right) >> 1;

        if (index < mid) {
            update(2 * v, left, mid, index, val);
        } else {
            update(2 * v + 1, mid, right, index, val);
        }

        tree[v] = max(tree[2 * v], tree[2 * v + 1]);

    }

    T get_max(int v, int left, int right, int req_l, int req_r) {

        if (req_l == left && req_r == right) {
            return tree[v];
        }

        int mid = (left + right) >> 1;
        T ret = -(1 << 30);

        if (req_l < mid) {
            ret = get_max(2 * v, left, mid, req_l, min(mid, req_r));
        }

        if (req_r > mid) {
            T tmp;
            tmp = get_max(2 * v + 1, mid, right, max(req_l, mid), req_r);
            ret = max(ret, tmp);
        }

        return ret;

    }

    int get_index(int req_l, int req_r, T req, int N) {

        int mid = (req_l + req_r) >> 1;

        if (req_l == req_r - 1 && req <= get_max(1, 1, N, req_l, req_r)) {
            return req_l;
        }

        if (req > get_max(1, 1, N, req_l, mid)) {
            return get_index(mid, req_r, req, N);
        }

        return get_index(req_l, mid, req, N);

    }

private:

    T* tree;

    void build(int v, int left, int right, T* a) {

        if (left == right - 1) {
            tree[v] = a[left];
            return;
        }

        int mid = (left + right) >> 1;

        build(2 * v, left, mid, a);
        build(2 * v + 1, mid, right, a);
        tree[v] = max(tree[2 * v], tree[2 * v + 1]);

    }

    int min(T a, T b) {
        return a < b ? a : b;
    }

    int max(T a, T b) {
        return a > b ? a : b;
    }

};


int main() {

    std::cin.tie(0);
    std::ios_base::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    // n = m = 200'000;

    auto *a = new int[n + 1];

    for (int i = 1; i <= n; ++i) {
        std::cin >> a[i];
        // a[i] = 200'000;
    }

    // a[3] = 1;

    Segment_tree<int> tree(n + 1, a);

    for (int j = 0; j < m; ++j) {

        int c, i, x;
        std::cin >> c >> i >> x;

        if (c == 1) {

            if (tree.get_max(1, 1, n + 1, i, n + 1) < x) {
                std::cout << -1 << '\n';
            } else {
                std::cout << tree.get_index(i, n + 1, x, n + 1) << '\n';
            }

        } else {
            tree.update(1, 1, n + 1, i, x);
        }

    }

    delete[] a;

}