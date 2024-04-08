// This solution is O(2^(n/2) * n), n <= 50 and 2978 ms / 3000 ms.
// It was only accepted on the second attempt of the same code :)
// And it doesn't pass on the third attempt...


#pragma GCC target("avx2")
#pragma GCC optimization("O3")
#pragma GCC optimization("unroll-loops")


#include <iostream>
#include <vector>


bool bit(int mask, int i) {
    return (mask >> i) & 1;
}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<std::vector<int>> g(n, std::vector<int>(n));

    // O(n^2)
    char c;
    for (auto& row : g) {
        for (auto& el : row) {
            std::cin >> c;
            el = c == '1' ? 1 : 0;
        }
    }

    int n_2 = n / 2;
    int n_1 = n - n_2;

    // O(n_1^2) = O(n^2)
    // neighbor_1_1[i] -- mask of neighbors of vertex i from first half in first half
    std::vector<int> neighbor_1_1(n_1, 0);
    for (int i = 0; i < n_1; ++i) {
        for (int j = 0; j < n_1; ++j) {
            if (g[i][j] == 1) {
                neighbor_1_1[i] |= 1 << j;
            }
        }
    }

    // O(n_1 * n_2) = O(n^2)
    // neighbor_1_2[i] -- mask of neighbors of vertex i from first half in second half
    std::vector<int> neighbor_1_2(n_1, 0);
    for (int i = 0; i < n_1; ++i) {
        for (int j = 0; j < n_2; ++j) {
            if (g[i][n_1 + j] == 1) {
                neighbor_1_2[i] |= 1 << j;
            }
        }
    }

    // O(n_2^2) = O(n^2)
    // neighbor_2_2[i] -- mask of neighbors of vertex i from second half in second half
    std::vector<int> neighbor_2_2(n_2, 0);
    for (int i = 0; i < n_2; ++i) {
        for (int j = 0; j < n_2; ++j) {
            if (g[n_1 + i][n_1 + j] == 1) {
                neighbor_2_2[i] |= 1 << j;
            }
        }
    }

    // O(2^n_2) = O(2^(n/2))
    int oldest = -1;
    std::vector<int> subclique_2(1 << n_2, 0);
    subclique_2[0] = 1;
    for (int mask = 1; mask < 1 << n_2; ++mask) {
        if (!(mask & (mask - 1))) {
            ++oldest;
        }
        if (subclique_2[mask ^ (1 << oldest)] == 1 &&
            (neighbor_2_2[oldest] | (mask ^ (1 << oldest))) == neighbor_2_2[oldest]) {
            subclique_2[mask] = 1;
        }
    }

    // O(2^n_2 * n_2) = O(2^(n/2) * n)
    for (int i = 0; i < n_2; ++i) {
        for (int mask = 0; mask < 1 << n_2; ++mask) {
            if (bit(mask, i)) {
                subclique_2[mask] += subclique_2[mask ^ (1 << i)];
            }
        }
    }

    // O(2^n_1 * n_1) = O(2^(n/2) * n)
    int64_t res = 0;
    int neighbor;
    bool f;
    for (int mask = 1; mask < 1 << n_1; ++mask) {
        neighbor = (1 << n_2) - 1;
        f = true;
        for (int i = 0; i < n_1; ++i) {
            if (bit(mask, i)) {
                if (((mask ^ (1 << i)) | neighbor_1_1[i]) != neighbor_1_1[i]) {
                    f = false;
                    break;
                }
                neighbor &= neighbor_1_2[i];
            }
        }
        if (f) {
            res += subclique_2[neighbor];
        }
    }

    res += subclique_2[(1 << n_2) - 1];

    std::cout << res << '\n';

}