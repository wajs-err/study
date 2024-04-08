#include <iostream>
#include <vector>
#include <numeric>


int main() {

    constexpr auto inf = std::numeric_limits<int>::max() / 2;

    size_t n;
    std::cin >> n;

    std::vector<std::vector<int>> g(n, std::vector<int>(n));

    for (auto& row : g) {
        for (auto& el : row) {
            std::cin >> el;
        }
    }

    std::vector<std::vector<int>> dp(n, std::vector<int>(1 << n, inf));
    std::vector<std::vector<size_t>> parent(n, std::vector<size_t>(1 << n, 0));

    for (size_t v = 0; v < n; ++v) {
        dp[v][1 << v] = 0;
    }

    for (size_t mask = 1; mask < (1 << n); ++mask) {
        for (size_t from = 0; from < n; ++from) {
            for (size_t to = 0; to < n; ++to) {
                if (to != from && (mask & (1 << to)) == 0) {
                    if (dp[from][mask] + g[from][to] < dp[to][mask | (1 << to)]) {
                        dp[to][mask | (1 << to)] = dp[from][mask] + g[from][to];
                        parent[to][mask | (1 << to)] = from;
                    }
                }
            }
        }
    }

    int min = inf;
    size_t index;
    for (size_t i = 0; i < n; ++i) {
        if (dp[i].back() < min) {
            min = dp[i].back();
            index = i;
        }
    }

    std::cout << min << '\n';


    size_t p = index;
    size_t mask = (1 << n) - 1;
    for (size_t i = 0; i < n; ++i) {
        std::cout << p + 1 << ' ';
        mask ^= (1 << p);
        p = parent[p][mask | (1 << p)];
    }

    std::cout << '\n';

}