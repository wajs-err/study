#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>


// Segment tree supports group increase operations and find min/count of mins operations iff all a_i >= 0
// or something else. I don't know.
class SegmentTree {

public:

    SegmentTree(int64_t n, const std::vector<int64_t>& x) : size(n), v(4 * n) {
        if (n != 0) {
            initialize(0, 0, n - 1, x);
        }
    }
    ~SegmentTree() = default;

    // increase value on [l, r]
    void increase(int64_t l, int64_t r, int64_t value) {
        increase(l, r, value, 0, 0, size - 1);
    }

    // find {min, count of mins} on [l, r]
    std::pair<int64_t, int64_t> min(int64_t l, int64_t r) {
        return min(l, r, 0, 0, size - 1);
    }

private:

    struct Node {
        int64_t value = 0;
        int64_t count = 0;
        int64_t promise = 0;
    };

    int64_t size;
    std::vector<Node> v;

    void increase(int64_t l, int64_t r, int64_t value, size_t index, int64_t tl, int64_t tr) {

        if (l == tl && r == tr) {
            v[index].value += value;
            v[index].promise += value;
            return;
        }

        push(index);

        int64_t m = (tl + tr) / 2;

        if (l <= m) {
            increase(l, std::min(m, r), value, 2 * index + 1, tl, m);
        }

        if (r > m) {
            increase(std::max(l, m + 1), r, value, 2 * index + 2, m + 1, tr);
        }

        if (v[2 * index + 1].value == v[2 * index + 2].value) {
            v[index].value = v[2 * index + 1].value;
            v[index].count = v[2 * index + 1].count + v[2 * index + 2].count;
            return;
        }

        v[2 * index + 1].value < v[2 * index + 2].value
            ? (v[index].value = v[2 * index + 1].value, v[index].count = v[2 * index + 1].count)
            : (v[index].value = v[2 * index + 2].value, v[index].count = v[2 * index + 2].count);

    }

    std::pair<int64_t, int64_t> min(int64_t l, int64_t r, size_t index, int64_t tl, int64_t tr) {

        if (l == tl && r == tr) {
            return {v[index].value, v[index].count};
        }

        int64_t tm = (tl + tr) / 2;
        int64_t minimum = std::numeric_limits<int64_t>::max();
        int64_t count = 0;

        push(index);

        if (l <= tm) {
            auto [m, c] = min(l, std::min(r, tm), 2 * index + 1, tl, tm);
            minimum = m;
            count = c;
        }

        if (r > tm) {
            auto [m, c] = min(std::max(l, tm + 1), r, 2 * index + 2, tm + 1, tr);
            if (minimum < m) {
                count = c;
            }
            if (minimum == m) {
                count += c;
            }
        }

        return {minimum, count};

    }

    void push(size_t index) {

        if (v[index].promise == 0) {
            return;
        }

        if (2 * index + 1 < v.size()) {
            v[2 * index + 1].value += v[index].promise;
            v[2 * index + 1].promise += v[index].promise;
        }

        if (2 * index + 2 < v.size()) {
            v[2 * index + 2].value += v[index].promise;
            v[2 * index + 2].promise += v[index].promise;
        }

        v[index].promise = 0;

    }

    void initialize(size_t index, int64_t tl, int64_t tr, const std::vector<int64_t>& x) {
        if (tl == tr) {
            v[index].count = tl == x.size() - 1 ? 1 : x[tl + 1] - x[tl];
            return;
        }
        int64_t m = (tl + tr) / 2;
        initialize(2 * index + 1, tl, m, x);
        initialize(2 * index + 2, m + 1, tr, x);
        v[index].count = v[2 * index + 1].count + v[2 * index + 2].count;
    }

};


int main() {

    struct S {
        int64_t x_1;
        int64_t y_1;
        int64_t x_2;
        int64_t y_2;
        bool is_end;
    };

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int64_t n;
    std::cin >> n;

    if (n == 0) {
        std::cout << "0\n";
        return 0;
    }

    std::vector<int64_t> xs(2 * n);
    std::vector<S> coords(2 * n);

    int64_t x_1, x_2, y_1, y_2;
    for (int64_t i = 0; i < n; ++i) {

        std::cin >> x_1 >> y_1 >> x_2 >> y_2;

        coords[i] = {.x_1 = x_1,
                     .y_1 = y_1,
                     .x_2 = x_2,
                     .y_2 = y_2,
                     .is_end = y_1 > y_2};

        coords[i + n] = {.x_1 = x_2,
                         .y_1 = y_2,
                         .x_2 = x_1,
                         .y_2 = y_1,
                         .is_end = !coords[i].is_end};

        xs[i] = x_1;
        xs[i + n] = x_2;

    }

    std::sort(xs.begin(), xs.end());
    xs.resize(std::unique(xs.begin(), xs.end()) - xs.begin());

    std::for_each(coords.begin(), coords.end(), [w = *xs.begin()](auto& x){ x.x_1 -= w; x.x_2 -= w; });
    std::for_each(xs.rbegin(), xs.rend(), [w = *xs.begin()](int64_t& x){ x -= w; });

    std::sort(coords.begin(), coords.end(), [](const S& lhs, const S& rhs){
        return lhs.y_1 < rhs.y_1 || lhs.y_1 == rhs.y_1 && lhs.x_1 < rhs.x_1;
    });

    SegmentTree s(xs.size(), xs);
    int64_t area = 0;

    int64_t i = 0;
    while (i < 2 * n) {

        do {

            int64_t l = std::lower_bound(xs.begin(), xs.end(), coords[i].x_1) - xs.begin();
            int64_t r = std::lower_bound(xs.begin(), xs.end(), coords[i].x_2) - xs.begin();

            if (l != r) {
                s.increase(std::min(l, r), std::max(l, r) - 1, coords[i].is_end ? -1 : 1);
            }

            ++i;

        } while (coords[i].y_1 == coords[i - 1].y_1);

        if (i != 2 * n) {
            area += (xs.back() + 1 - s.min(0, xs.size() - 1).second) * (coords[i].y_1 - coords[i - 1].y_1);
        }

    }

    std::cout << area << '\n';

    return 0;

}