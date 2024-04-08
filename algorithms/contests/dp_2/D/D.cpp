#include <iostream>
#include <vector>


constexpr uint64_t mod = 1'000'000'007;


uint64_t count_ones(uint64_t x) {

    uint64_t count = 0;

    while (x > 0) {
        if (x % 2 != 0) {
            ++count;
        }
        x /= 2;
    }

    return count % 3 == 0;

}


void operator*=(std::vector<std::vector<uint64_t>>& lhs, const std::vector<std::vector<uint64_t>>& rhs) {

    auto size = lhs.size();

    std::vector<std::vector<uint64_t>> answer(size, std::vector<uint64_t>(size, 0));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            for (size_t k = 0; k < size; ++k) {
                (answer[i][j] += lhs[i][k] * rhs[k][j] % mod) %= mod;
            }
        }
    }

    lhs = std::move(answer);

}



int main() {

    size_t n, k;
    std::cin >> n >> k;
    --k;

    std::vector<uint64_t> a(n);

    for (auto& el : a) {
        std::cin >> el;
    }

    std::vector<std::vector<uint64_t>> matrix(n, std::vector<uint64_t>(n));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            matrix[i][j] = count_ones(a[i] ^ a[j]);
            matrix[j][i] = matrix[i][j];
        }
    }

    std::vector<std::vector<uint64_t>> M(n, std::vector<uint64_t>(n, 0));
    for (auto i = 0; i < n; ++i) {
        M[i][i] = 1;
    }

    while (k != 0) {

        if (k % 2 != 0) {
            M *= matrix;
        }

        matrix *= matrix;
        k /= 2;

    }

    uint64_t sum = 0;

    for (auto& row: M) {
        for (auto& el: row) {
            (sum += el) %= mod;
        }
    }

    std::cout << sum << '\n';


}