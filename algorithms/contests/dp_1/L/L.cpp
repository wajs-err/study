// This solution is 1107 ms, but there is 15 ms solution :)
// Last one for is very slow -- O(2^n). It can probably be faster


#include <iostream>
#include <vector>


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t n;
    std::cin >> n;

    std::vector<int64_t> v(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> v[i];
    }

    int64_t C;
    std::cin >> C;

    size_t n_1 = n / 2;
    std::vector<int64_t> s_1(1 << n_1, 0);
    for (size_t i = 0; i < (1 << n_1); ++i) {
        for (size_t j = 0; j < n_1; ++j) {
            if ((i & (1 << j)) != 0) {
                s_1[i] += v[j];
            }
        }
    }

    size_t n_2 = n - n_1;
    std::vector<int64_t> s_2(1 << n_2, 0);
    for (size_t i = 0; i < (1 << n_2); ++i) {
        for (size_t j = 0; j < n_2; ++j) {
            if ((i & (1 << j)) != 0) {
                s_2[i] += v[n_1 + j];
            }
        }
    }

    size_t count = 0;
    for (auto& e_1 : s_1) {
        for (auto& e_2 : s_2) {
            if (e_1 + e_2 <= C) {
                ++count;
            }
        }
    }

    std::cout << count << '\n';

}