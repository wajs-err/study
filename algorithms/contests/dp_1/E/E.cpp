// This solution is 655 ms, but there is 15 ms solution :)
// This solution is O(n^2) or O(n^3) (and it is my own)


#include <iostream>
#include <vector>


int main() {

    size_t n;
    std::cin >> n;

    // dp[i][j] is number of peaceful sets with sum j and max element i
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));

    dp[1][1] = 1;

    for (size_t i = 2; i <= n; ++i) {
        for (size_t j = i; j <= n; ++j) {

            if (i == j) {
                dp[i][j] = 1;
                continue;
            }

            for (size_t k = 1; k <= i / 2; ++k) {
                dp[i][j] += dp[k][j - i];
            }

            if (dp[i][j] == 0) {
                break;
            }

        }
    }

    int64_t ans = 0;
    for (size_t i = 1; i <= n; ++i) {
        ans += static_cast<int64_t>(dp[i].back());
    }

    std::cout << ans << '\n';

}