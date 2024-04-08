#include <iostream>
#include <vector>


int main() {

    size_t n;
    std::cin >> n;
    std::vector<int> v_1(n);
    for (int& el : v_1) {
        std::cin >> el;
    }

    size_t m;
    std::cin >> m;
    std::vector<int> v_2(m);
    for (int& el : v_2) {
        std::cin >> el;
    }

    std::vector<int> prev(m + 1, 0);
    std::vector<int> curr(m + 1, 0);

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            curr[j] = std::max(v_1[i - 1] == v_2[j - 1] ? prev[j - 1] + 1 : 0, std::max(prev[j], curr[j - 1]));
        }
        std::swap(prev, curr);
    }

    std::cout << prev.back() << '\n';

}
