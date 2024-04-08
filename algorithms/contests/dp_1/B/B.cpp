#include <iostream>
#include <vector>
#include <numeric>

int main() {

    size_t n;
    std::cin >> n;

    std::vector<int> v(n);
    std::vector<int> d(n + 1, std::numeric_limits<int>::max());
    std::vector<size_t> pos(n + 1, 0);
    std::vector<size_t> prev(n + 1, 0);
    d[0] = std::numeric_limits<int>::min();

    for (int& el : v) {
        std::cin >> el;
    }

    std::reverse(v.begin(), v.end());

    size_t max_length = 0;
    size_t index;
    for (size_t i = 0; i < n; ++i) {

        auto it = std::upper_bound(d.begin(), d.end(), v[i]);
        if (it != d.end()) {
            index = std::distance(d.begin(), it);
            *it = v[i];
            pos[index] = i;
            prev[i] = pos[index - 1];
            max_length = std::max(max_length, index);
        }

    }

    std::cout << max_length << '\n';

    size_t i = pos[max_length];
    for (size_t j = 0; j < max_length; ++j) {
        std::cout << n - i << ' ';
        i = prev[i];
    }

    std::cout << '\n';

}