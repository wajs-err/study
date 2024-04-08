#include <iostream>
#include <limits>
#include <vector>


bool bit(int mask, int i) {
    return (mask >> i) & 1;
}


// A / B
int relative_complement(int A, int B) {
    return (A ^ B) & A;
}


int main() {

    int n, m;
    std::cin >> n >> m;

    std::vector<int> dists(n);
    std::vector<std::vector<int>> costs(n, std::vector<int>(m));

    for (int i = 0; i < n; ++i) {
        std::cin >> dists[i];
        for (auto& el : costs[i]) {
            std::cin >> el;
        }
    }

    // best[mask] -- the least sum to buy goods from mask in one shop
    std::vector<int> best(1 << m, std::numeric_limits<int>::max());
    std::vector<int> mask_vector(m);
    int sum;
    best[0] = 0;
    for (int mask = 1; mask < 1 << m; ++mask) {

        mask_vector.clear();

        for (int j = 0; j < m; ++j) {
            if (bit(mask, j)) {
                mask_vector.push_back(j);
            }
        }

        for (int j = 0; j < n; ++j) {
            sum = dists[j];
            for (auto& el : mask_vector) {
                sum += costs[j][el];
            }
            best[mask] = std::min(best[mask], sum);
        }

    }

    // best[mask] -- the least sum to buy goods from mask in any shops
    std::vector<int> dp(1 << m);

    for (int mask = 0; mask < 1 << m; ++mask){
        dp[mask] = best[mask];
        for(int submask = mask; submask > 0; submask = (submask - 1) & mask){
            dp[mask] = std::min(dp[mask], dp[submask] + dp[relative_complement(mask, submask)]);
        }
    }

    std::cout << dp.back() << '\n';

}