#include <iostream>
#include <limits>
#include <vector>


class SegmentTree {

public:

    explicit SegmentTree(int n) : size(n), v(4 * n) {}
    ~SegmentTree() = default;

    // assign value on [l, r]
    void assign(int l, int r, int value) {
        assign(l, r, value, 0, 0, size - 1);
    }

    // find min on [l, r]
    int min(int l, int r) {
        return min(l, r, 0, 0, size - 1);
    }

private:

    struct Node {
        int value = -1;
        int promise = -1;
    };

    int size;
    std::vector<Node> v;

    void assign(int l, int r, int value, size_t index, int tl, int tr) {

        if (l == tl && r == tr) {
            v[index].promise = value;
            v[index].value = value;
            return;
        }

        push(index);

        int m = (tl + tr) / 2;

        if (l <= m) {
            assign(l, std::min(m, r), value, 2 * index + 1, tl, m);
        }

        if (r > m) {
            assign(std::max(l, m + 1), r, value, 2 * index + 2, m + 1, tr);
        }

        v[index].value = std::min(v[2 * index + 1].value, v[2 * index + 2].value);

    }

    int min(int l, int r, size_t index, int tl, int tr) {

        if (l == tl && r == tr) {
            return v[index].value;
        }

        int m = (tl + tr) / 2;
        int res = std::numeric_limits<int>::max();

        push(index);

        if (l <= m) {
            res = std::min(res, min(l, std::min(r, m), 2 * index + 1, tl, m));
        }

        if (r > m) {
            res = std::min(res, min(std::max(l, m + 1), r, 2 * index + 2, m + 1, tr));
        }

        return res;

    }

    void push(size_t index) {

        if (v[index].promise == -1) {
            return;
        }

        if (2 * index + 1 < v.size()) {
            v[2 * index + 1].value = v[index].promise;
            v[2 * index + 1].promise = v[index].promise;
        }

        if (2 * index + 2 < v.size()) {
            v[2 * index + 2].value = v[index].promise;
            v[2 * index + 2].promise = v[index].promise;
        }

        v[index].promise = -1;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    SegmentTree s(n);

    int r, g, b;
    for (int i = 0; i < n; ++i) {
        std::cin >> r >> g >> b;
        s.assign(i, i, r + g + b);
    }

    int k;
    std::cin >> k;

    int c, d;
    int e, f;
    for (int i = 0; i < k; ++i) {
        std::cin >> c >> d >> r >> g >> b >> e >> f;
        s.assign(c, d, r + g + b);
        std::cout << s.min(e, f) << ' ';
    }

}