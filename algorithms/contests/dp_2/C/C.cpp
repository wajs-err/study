#include <algorithm>
#include <iostream>
#include <vector>


using std::vector;
using Matrix = vector<vector<int64_t>>;
constexpr int mod = 1'000'000'000 + 7;


Matrix build_matrix(vector<int64_t>& v) {

    auto size = v.size();

    std::reverse(v.begin() + 1, v.end());

    Matrix res(size);

    for (auto& row : res) {
        row = v;
        std::rotate(v.rbegin(), v.rbegin() + 1, v.rend());
    }

    return res;

}


void multiply(Matrix& lhs, const Matrix& rhs) {

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


void power(Matrix& lhs, int64_t power) {

    auto size = lhs.size();
    auto copy = lhs;

    std::vector<std::vector<int64_t>> M(size, std::vector<int64_t>(size, 0));
    for (auto i = 0; i < size; ++i) {
        M[i][i] = 1;
    }

    while (power != 0) {

        if (power % 2 != 0) {
            multiply(M, copy);
        }

        multiply(copy, copy);
        power /= 2;

    }

    lhs = std::move(M);

}


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    int64_t n, l, m;
    std::cin >> n >> l >> m;

    std::vector<int64_t> begin(m, 0);
    std::vector<int64_t> middle(m, 0);
    std::vector<int64_t> end(m, 0);
    std::vector<int64_t> weight(n, 0);

    int c;

    for (int i = 0; i < n; ++i) {
        std::cin >> c;
        ++begin[c % m];
    }

    for (int i = 0; i < n; ++i) {
        std::cin >> c;
        weight[i] = c;
        ++middle[c % m];
    }

    for (int i = 0; i < n; ++i) {
        std::cin >> c;
        ++end[(c + weight[i]) % m];
    }

    Matrix Middle = build_matrix(middle);
    Matrix End = build_matrix(end);
    power(Middle, l - 2);
    multiply(Middle, End);

    int64_t res = 0;
    for (auto i = 0; i < begin.size(); ++i) {
        (res += Middle[0][i] * begin[i] % mod) %= mod;
    }
    std::cout << res << '\n';

}