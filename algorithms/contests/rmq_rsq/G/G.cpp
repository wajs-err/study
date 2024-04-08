// This code is very non-optimal. Especially WiredFenwickTree constructor.


#include <algorithm>
#include <iostream>
#include <vector>


struct Point {
    int x;
    int y;
    int w;
};


class FenwickTree {

public:

    FenwickTree() = delete;
    explicit FenwickTree(size_t n) : v(n) {}
    ~FenwickTree() = default;

    uint64_t sum(int x) {
        uint64_t res = 0;
        if (x >= v.size()) {
            x = v.size() - 1;
        }
        for (int i = x; i >= 0; i = f(i) - 1) {
            res += v[i];
        }
        return res;
    }

    void update(int x, int w) {
        for (int i = x; i < v.size(); i = g(i)) {
            v[i] += w;
        }
    }

private:

    std::vector<int64_t> v;

    static int f(int x) {
        return x & (x + 1);
    }

    static int g(int x) {
        return x | (x + 1);
    }

};


// aka wierd 2d Fenwick tree
// aka Fenwick tree of Fenwick trees
// aka compressed Fenwick tree
// aka ФырФырФенвик
class WeiredFenwickTree {

public:

    explicit WeiredFenwickTree(const std::vector<Point>& coords) {

        x_coords.push_back(coords[0].x);
        for (size_t i = 1; i < coords.size(); ++i) {
            if (coords[i].x != x_coords.back()) {
                x_coords.push_back(coords[i].x);
            }
        }

        int x_size = x_coords.size();
        y_coords.resize(x_size);
        std::vector<std::vector<int>> weights(x_size);

        for (const auto& [x, y, w] : coords) {
            for (int i = get_compressed_x(x); i < x_size; i = g(i)) {
                y_coords[i].push_back(y);
                weights[i].push_back(w);
            }
        }

        for (int i = 0; i < x_coords.size(); ++i) {

            auto first = y_coords[i].begin();
            auto last = y_coords[i].end();

            if (first == last) {
                continue;
            }

            auto result = first;

            while (++first != last) {
                if (*result != *first && ++result != first) {
                    *result = *first;
                    weights[i][result - y_coords[i].begin()] = weights[i][first - y_coords[i].begin()];
                } else if (result != first) {
                    weights[i][result - y_coords[i].begin()] += weights[i][first - y_coords[i].begin()];
                }
            }

            auto new_size = std::unique(y_coords[i].begin(), y_coords[i].end()) - y_coords[i].begin();
            y_coords[i].resize(new_size);
            weights[i].resize(new_size);

        }

        std::vector<std::vector<std::pair<int, int>>> tmp(x_size);

        for (int i = 0; i < x_size; ++i) {
            for (int j = 0; j < y_coords[i].size(); ++j) {
                tmp[i].emplace_back(y_coords[i][j], weights[i][j]);
            }
        }

        for (auto& vec : tmp) {
            std::sort(vec.begin(), vec.end());
        }

        for (int i = 0; i < x_size; ++i) {
            for (int j = 0; j < y_coords[i].size(); ++j) {
                y_coords[i][j] = tmp[i][j].first;
                weights[i][j] = tmp[i][j].second;
            }
        }

        for (int i = 0; i < x_size; ++i) {
            tree.emplace_back(y_coords[i].size());
            for (int j = 0; j < y_coords[i].size(); ++j) {
                tree[i].update(get_compressed_y(y_coords[i][j], i), weights[i][j]);
            }
        }

    }

    ~WeiredFenwickTree() = default;

    int64_t sum(int x, int y) {
        x = get_compressed_x(x);
        int64_t res = 0;
        int y_;
        for (int i = x; i >= 0; i = f(i) - 1) {
            y_ = get_compressed_y(y, i);
            if (y_ >= 0) {
                res += tree[i].sum(y_);
            }
        }
        return res;
    }

    void update(int x, int y, int w) {
        x = get_compressed_x(x);
        for (int i = x, y_; i < tree.size(); i = g(i)) {
            y_ = get_compressed_y(y, i);
            if (y_ >= 0) {
                tree[i].update(y_, w);
            }
        }
    }

private:

    std::vector<FenwickTree> tree;
    std::vector<std::vector<int>> y_coords;
    std::vector<int> x_coords;

    static int f(int x) {
        return x & (x + 1);
    }

    static int g(int x) {
        return x | (x + 1);
    }

    int get_compressed_x(int x) {
        auto it = std::lower_bound(x_coords.begin(), x_coords.end(), x);
        if (it == x_coords.end() || *it > x) {
            --it;
        }
        return it - x_coords.begin();
    }

    int get_compressed_y(int y, int index) {
        auto it = std::lower_bound(y_coords[index].begin(), y_coords[index].end(), y);
        if (it == y_coords[index].end() || *it > y) {
            --it;
        }
        return it - y_coords[index].begin();
    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<Point> v(n);

    for (auto& [x, y, w] : v) {
        std::cin >> x >> y >> w;
    }

    std::vector<Point> vv = v;

    std::sort(v.begin(), v.end(), [](const Point& lhs, const Point& rhs){
        return lhs.x < rhs.x ||
                lhs.x == rhs.x && lhs.y < rhs.y ||
                lhs.x == rhs.x && lhs.y == rhs.y && lhs.w < rhs.w;
    });

    WeiredFenwickTree t(v);

    int m;
    std::cin >> m;

    std::string s;
    int x, y, num, w;
    for (int i = 0; i < m; ++i) {
        std::cin >> s;
        if (s[0] == 'g') {
            std::cin >> x >> y;
            std::cout << t.sum(x, y) << '\n';
            continue;
        }
        std::cin >> num >> w;
        --num;
        t.update(vv[num].x, vv[num].y, w - vv[num].w);
        vv[num].w = w;
    }

}