#include <iostream>
#include <numeric>
#include <vector>


int main() {

    constexpr int inf = std::numeric_limits<int>::max() / 2;

    size_t n, m;
    std::cin >> n >> m;

    std::vector<int> v(n);

    for (auto& el : v) {
        std::cin >> el;
    }

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n, inf));
    std::vector<std::vector<size_t>> from(m + 1, std::vector<size_t>(n, inf));

    for (size_t i = 0; i < n; ++i) {
        dp[0][i] = 0;
    }

    dp[1][0] = 0;
    for (int j = 1; j < n; ++j) {
        dp[1][j] = dp[1][j - 1] + j * (v[j] - v[j - 1]);
        from[1][j] = 0;
    }

    for (size_t i = 2; i <= m; ++i) {
        for (size_t j = 0; j < n; ++j) {

            // min_\limits k \in [0; j) dp[i - 1][k] + dist(k, j)
            // dist(k, j) -- sum of distances from all of the villages in (k, j)

            for (size_t k = 0; k < j; ++k) {

                int dist = 0;

                for (size_t l = k + 1; l < j; ++l) {
                    dist += std::min(v[l] - v[k], v[j] - v[l]);
                }

                if (dp[i - 1][k] + dist < dp[i][j]) {
                    dp[i][j] = dp[i - 1][k] + dist;
                    from[i][j] = k;
                }

            }

        }
    }

    int min = inf;
    size_t index;
    for (size_t j = 0 ;j < n; ++j) {
        int right = 0;
        for (size_t k = j + 1; k < n; ++k) {
            right += v[k] - v[j];
        }
        if (dp.back()[j] + right < min) {
            min = dp.back()[j] + right;
            index = j;
        }
    }

    std::cout << min << '\n';

    std::vector<int> ans;
    size_t i = m;
    while (index != 0) {
        ans.push_back(v[index]);
        index = from[i--][index];
    }

    std::reverse(ans.begin(), ans.end());
    for (auto& el : ans) {
        std::cout << el << ' ';
    }

    std::cout << '\n';

}