#include <iostream>
#include <vector>
#include <algorithm>


int main() {

    size_t s;
    size_t n;
    std::cin >> s >> n;

    std::vector<int> v(n + 1);
    for (size_t i = 1; i <= n; ++i) {
        std::cin >> v[i];
    }

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(s + 1, 0));

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 0; j <= s; ++j) {
            dp[i][j] = std::max(dp[i - 1][j],
                                j >= v[i] ? dp[i - 1][j - v[i]] + v[i] : 0);
        }
    }

    std::cout << dp[n][s] << '\n';

}