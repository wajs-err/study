#include <iostream>
#include <vector>


int bit(int mask, int i) {
    return (mask & (1 << i)) >> i;
}


int main() {

    int n, m;
    std::cin >> n >> m;

    if (m < n) {
        std::swap(n, m);
    }

    std::vector<std::vector<bool>> is_ok(1 << n, std::vector<bool>(1 << n, true));

    for (int mask_1 = 0; mask_1 < 1 << n; ++mask_1) {
        for (int mask_2 = mask_1; mask_2 < 1 << n; ++mask_2) {
            for (int i = 0; i < n - 1; ++i) {
                if (bit(mask_1, i) == bit(mask_2, i) &&
                    bit(mask_1, i) == bit(mask_1, i + 1) &&
                    bit(mask_1, i + 1) == bit(mask_2, i + 1)) {
                    is_ok[mask_1][mask_2] = false;
                    is_ok[mask_2][mask_1] = false;
                }
            }
        }
    }

    std::vector<std::vector<int>> dp(m, std::vector<int>(1 << n, 0));
    for (int mask = 0; mask < 1 << n; ++mask) {
        dp[0][mask] = 1;
    }

    for (auto i = 1; i < m; ++i) {
        for (int curr_mask = 0; curr_mask < 1 << n; ++curr_mask) {
            for (int new_mask = 0; new_mask < 1 << n; ++new_mask) {
                if (is_ok[curr_mask][new_mask]) {
                    dp[i][new_mask] += dp[i - 1][curr_mask];
                }
            }
        }
    }

    int res = 0;
    for (auto& el : dp.back()) {
        res += el;
    }
    std::cout << res << '\n';

}