#include <iostream>
#include <vector>


constexpr int mod = 999'999'937;


void operator*=(std::vector<std::vector<uint64_t>>& lhs, const std::vector<std::vector<uint64_t>>& rhs) {

    std::vector<std::vector<uint64_t>> answer(5, std::vector<uint64_t>(5, 0));

    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            for (size_t k = 0; k < 5; ++k) {
                (answer[i][j] += lhs[i][k] * rhs[k][j] % mod) %= mod;
            }
        }
    }

    lhs = std::move(answer);

}


int main() {

    for ( ; ; ) {

        size_t n;
        std::cin >> n;

        if (n == 0) {
            break;
        }

        --n;

        std::vector<std::vector<uint64_t>> matrix(5, std::vector<uint64_t>(5, 1));
        matrix[3][2] = 0;
        matrix[3][4] = 0;
        matrix[4][2] = 0;
        matrix[4][4] = 0;

        std::vector<std::vector<uint64_t>> M(5, std::vector<uint64_t>(5, 0));
        for (auto i = 0; i < 5; ++i) {
            M[i][i] = 1;
        }

        while (n != 0) {

            if (n % 2 != 0) {
                M *= matrix;
            }

            matrix *= matrix;
            n /= 2;

        }

        uint64_t sum = 0;

        for (auto &row: M) {
            for (auto &el: row) {
                (sum += el) %= mod;
            }
        }

        std::cout << sum << '\n';

    }

}