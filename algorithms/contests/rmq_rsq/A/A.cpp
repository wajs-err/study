#include <iostream>


template <typename T> // [l; r)
class Segment_tree {

public:

    Segment_tree(int n, T *a): tree(new std::pair<T, T>[4 * n + 1]) {
        build(1, 1, n, a);
    }

    ~Segment_tree() {
        delete[] tree;
    }

    void update(int v, int left, int right, int index, T val) {

        if (left == right - 1) {
            tree[v] = {val, val};
            return;
        }

        int mid = (left + right) >> 1;

        if (index < mid) {
            update(2 * v, left, mid, index, val);
        } else {
            update(2 * v + 1, mid, right, index, val);
        }

        tree[v].first = max(tree[2 * v].first, tree[2 * v + 1].first);
        tree[v].second = min(tree[2 * v].second, tree[2 * v + 1].second);

        std::pair<T, T> test = tree[v];

    }

    std::pair<T, T> get_minmax(int v, int left, int right, int req_l, int req_r) {

        if (req_l == left && req_r == right) {
            return tree[v];
        }

        int mid = (left + right) >> 1;
        std::pair<T, T> ret = {-(1 << 30), 1 << 30};

        if (req_l < mid) {
            ret = get_minmax(2 * v, left, mid, req_l, min(mid, req_r));
        }

        if (req_r > mid) {
            std::pair<T, T> tmp;
            tmp = get_minmax(2 * v + 1, mid, right, max(req_l, mid), req_r);
            ret.first = max(ret.first, tmp.first);
            ret.second = min(ret.second, tmp.second);
        }

        return ret;

    }

    void print() {
        for (int i = 0; i < 13; ++i) {
            std::cout << tree[i].first << ' ' << tree[i].second << '\n';
        }
    }

private:

    std::pair<T, T>* tree;

    void build(int v, int left, int right, T *a) {

        if (left == right - 1) {
            tree[v] = {a[left], a[left]};
            return;
        }

        int mid = (left + right) >> 1;

        build(2 * v, left, mid, a);
        build(2 * v + 1, mid, right, a);
        tree[v].first = max(tree[2 * v].first, tree[2 * v + 1].first);
        tree[v].second = min(tree[2 * v].second, tree[2 * v + 1].second);

    }

    int min(T a, T b) {
        return a < b ? a : b;
    }

    int max(T a, T b) {
        return a > b ? a : b;
    }

};


// first - max
// second - min


int main() {

    constexpr int BN = 100000;

    auto* a = new long long[BN + 1];

    for (unsigned long long i = 1; i <= BN; ++i) {
        a[i] = i * i % 12345 + i * i * i % 23456;
    }

    Segment_tree<long long> tree(BN + 1, a);

    int m;
    std::cin >> m;

    for (int i = 0; i < m; ++i) {

        int x, y;
        std::cin >> x >> y;

        if (x > 0) {
            auto ans = tree.get_minmax(1, 1, BN + 1, x, y + 1);
            std::cout << ans.first - ans.second << '\n';
        } else {
            tree.update(1, 1, BN + 1, abs(x), y);
        }

    }

    delete[] a;

}