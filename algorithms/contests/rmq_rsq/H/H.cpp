#include <iostream>
#include <vector>
#include <algorithm>


int floor_log(int n) {

    int i = 0;

    while (n > 0) {
        n >>= 1;
        ++i;
    }

    return --i;

}


int get_second(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c, std::pair<int, int> d) {
    std::vector<std::pair<int, int>> x = {a, b, c, d};
    auto last = std::unique(x.begin(), x.end());
    x.erase(last, x.end());
    std::sort(x.begin(), x.end());
    return x[1].first;
}


int main() {

    int n, m;
    std::cin >> n >> m;

    std::vector< std::vector< std::pair<std::pair<int, int>, std::pair<int, int>> > > sparse
            (n, std::vector< std::pair<std::pair<int, int>, std::pair<int, int>> > (floor_log(n) + 1));

    for (int i = 0; i < n; ++i) {
        int z;
        std::cin >> z;
        sparse[i][0] = {{z, i}, {z, i}};
    }

    for (int i = 0; i < n; ++i) {

        if (i + 1 < n) {
            sparse[i][1] = {std::min(sparse[i][0].first, sparse[i + 1][0].first),
                            std::max(sparse[i][0].first, sparse[i + 1][0].first)};
        } else {
            sparse[i][1] = {{1'000'000'000, -1}, {1'000'000'000, -1}};
        }

    }

    for (int k = 1; k < floor_log(n); ++k) {

        int t = 1 << k;

        for (int i = 0; i < n; ++i) {
            if (i + (1 << k + 1) < n + 1) {
                sparse[i][k + 1].first = std::min(sparse[i][k].first, sparse[i + t][k].first);
                auto second = std::min(sparse[i][k].second, sparse[i + t][k].second);
                auto first = std::max(sparse[i][k].first, sparse[i + t][k].first);
                sparse[i][k + 1].second = std::min(first, second);

            } else {
                sparse[i][k + 1] = {{1'000'000'000, -1}, {1'000'000'000, -1}};
            }
        }

    }

    for (int i = 0; i < m; ++i) {

        int l, r;
        std::cin >> l >> r;
        --l; --r;

        int k = floor_log(r - l + 1);
        r = r - (1 << k) + 1;

        if (l != r) {
            std::cout << get_second(sparse[l][k].first, sparse[r][k].first, sparse[l][k].second, sparse[r][k].second) << '\n';
        } else {
            std::cout << sparse[l][k].second.first << '\n';
        }

    }

}