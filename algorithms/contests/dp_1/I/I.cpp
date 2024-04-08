#include <iostream>
#include <vector>


int main() {

    size_t n;
    size_t m;
    std::cin >> n >> m;

    std::vector<int> v_1(n);
    for (int& el : v_1) {
        std::cin >> el;
    }

    std::vector<int> v_2(m);
    for (int& el : v_2) {
        std::cin >> el;
    }

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

    int best;
    for (size_t i = 1; i <= n; ++i) {

        best = 0;
        for (size_t j = 1; j <= m; ++j) {

            dp[i][j] = dp[i - 1][j];

            if (v_1[i - 1] == v_2[j - 1] && dp[i - 1][j] < best + 1) {
                dp[i][j] = best + 1;
            }

            if (v_1[i - 1] > v_2[j - 1] && dp[i - 1][j] > best) {
                best = dp[i - 1][j];
            }

        }
    }

    int mx = 0;
    for (auto& vec : dp) {
        for (auto& el : vec) {
            if (el > mx) {
                mx = el;
            }
        }
    }

    std::cout << mx << '\n';

}