#include <iostream>
#include <vector>


using std::vector;
using Matrix = vector<vector<int64_t>>;


int64_t bit(int64_t mask, int64_t i) {
    return (mask >> i) & 1;
}


void initialize(Matrix& is_ok, int64_t n) {

    for (int mask_1 = 0; mask_1 < 1 << n; ++mask_1) {
        for (int mask_2 = mask_1; mask_2 < 1 << n; ++mask_2) {
            for (int i = 0; i < n - 1; ++i) {
                if (bit(mask_1, i) == bit(mask_2, i) &&
                    bit(mask_1, i) == bit(mask_1, i + 1) &&
                    bit(mask_1, i + 1) == bit(mask_2, i + 1)) {
                    is_ok[mask_1][mask_2] = 0;
                    is_ok[mask_2][mask_1] = 0;
                }
            }
        }
    }

}


void multiply(Matrix& lhs, const Matrix& rhs, int64_t mod) {

    auto size = lhs.size();

    vector<vector<int64_t>> answer(size, vector<int64_t>(size, 0));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            for (size_t k = 0; k < size; ++k) {
                (answer[i][j] += lhs[i][k] * rhs[k][j] % mod) %= mod;
            }
        }
    }

    lhs = std::move(answer);

}


void power(Matrix& lhs, int64_t power, int64_t mod) {

    auto size = lhs.size();

    std::vector<std::vector<int64_t>> M(size, std::vector<int64_t>(size, 0));
    for (auto i = 0; i < size; ++i) {
        M[i][i] = 1;
    }

    while (power != 0) {

        if (power % 2 != 0) {
            multiply(M, lhs, mod);
        }

        multiply(lhs, lhs, mod);
        power /= 2;

    }

    lhs = std::move(M);

}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int64_t t;
    std::cin >> t;

    int64_t n, m, mod;
    for (auto j = 0; j < t; ++j) {

        std::cin >> n >> m >> mod;
        std::swap(n, m);

        Matrix is_ok(1 << n, vector<int64_t>(1 << n, 1));
        initialize(is_ok, n);
        power(is_ok, m - 1, mod);

        int64_t res = 0;
        for (auto& row : is_ok) {
            for (auto& el : row) {
                (res += el) %= mod;
            }
        }
        std::cout << res << '\n';

    }

}