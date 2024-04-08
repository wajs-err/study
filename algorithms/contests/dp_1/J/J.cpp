#include <iostream>
#include <vector>


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    const unsigned bn = 1'000'000'000 + 7;

    size_t n;
    std::cin >> n;

    std::vector<size_t> v(n);
    size_t mx = 0;
    for (size_t& el : v) {
        std::cin >> el;
        if (el > mx) {
            mx = el;
        }
    }

    std::vector<unsigned> count(mx + 1, 0);

    unsigned res = 1;
    unsigned tmp;
    for (size_t& el : v) {
        tmp = (res + bn - count[el]) % bn;
        count[el] = res;
        (res += tmp) %= bn;
    }

    std::cout << (res != 0 ? res - 1 : bn - 1) << '\n';

}