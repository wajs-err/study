#include <iostream>
#include <vector>


std::vector<std::vector<int64_t>> matrix(17);
constexpr int64_t mod = 1'000'000'000 + 7;


void multiply(std::vector<std::vector<int64_t>>& lhs, const std::vector<std::vector<int64_t>>& rhs, int64_t size) {

    std::vector<std::vector<int64_t>> answer(size, std::vector<int64_t>(size, 0));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            for (size_t k = 0; k < size; ++k) {
                (answer[i][j] += lhs[i][k] * rhs[k][j] % mod) %= mod;
            }
        }
    }

    lhs = std::move(answer);

}


void handle_segment(std::vector<int64_t>& v, int64_t a, int64_t b, int64_t c) {

    auto size = c + 1;
    auto k = b - a;

    auto copy = matrix;
    std::vector<std::vector<int64_t>> M(size, std::vector<int64_t>(size, 0));
    for (auto i = 0; i < size; ++i) {
        M[i][i] = 1;
    }

    while (k != 0) {

        if (k % 2 != 0) {
            multiply(M, copy, size);
        }

        multiply(copy, copy, size);
        k /= 2;

    }

    std::vector<int64_t> vv(size, 0);
    for (auto i = 0; i < size; ++i) {
        for (auto j = 0; j < size; ++j) {
            (vv[i] += (v[j] * M[i][j]) % mod) %= mod;
        }
    }

    v = std::move(vv);

}


// c_max = 16
int main() {

    std::vector<int64_t> temp = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    for (auto& row : matrix) {
        row = temp;
        std::rotate(temp.rbegin(), temp.rbegin() + 1, temp.rend());
    }
    matrix.front().back() = 0;
    matrix.back().front() = 0;

    int64_t n, k;
    std::cin >> n >> k;

    std::vector<int64_t> v(17, 0);
    v[0] = 1;

    int64_t a, b, c, prev_c = 18;
    for (auto i = 0; i < n; ++i) {

        std::cin >> a >> b >> c;

        if (a > k) {
            continue;
        }

        if (a == b) {
            prev_c = std::min(c + 1, static_cast<int64_t>(v.size()));
            v.resize(prev_c);
            continue;
        }

        std::vector<int64_t> vv(c + 1, 0);
        if (prev_c < c) {
            for (int64_t j = 0; j < v.size(); ++j) {
                vv[j] = (j - 1 >= 0 ? v[j - 1] : 0) +
                        v[j] +
                        (j + 1 < v.size() ? v[j + 1] : 0);
            }
            vv[v.size()] = v.back();
        } else {
            v.resize(c + 1); // v.size() == vv.size()
            for (int64_t j = 0; j < vv.size(); ++j) {
                vv[j] = (j - 1 >= 0 ? v[j - 1] : 0) +
                        v[j] +
                        (j + 1 < v.size() ? v[j + 1] : 0);
            }
        }
        v = std::move(vv);

        if (b > k) {
            b = k;
        }

        handle_segment(v, a + 1, b, c);
        prev_c = c;

    }

    std::cout << v[0] << '\n';

}