#include <iostream>
#include <vector>


constexpr int64_t mod = 1'000'000'000 + 7;


int64_t substring_hash(const std::vector<int64_t>& hash, size_t l, size_t r, const std::vector<int64_t>& powers) {
    return (mod + hash[r] - (l != 0 ? hash[l - 1] : 0)) * powers[hash.size() - r - 1] % mod;
}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t n;
    int64_t m;

    std::cin >> n >> m;
    ++m;

    std::vector<int64_t> powers(n);
    for (int64_t p = 1; auto& el : powers) {
        el = p;
        (p *= m) %= mod;
    }

    std::vector<int64_t> hash(n);
    std::vector<int64_t> reverse_hash(n);
    for (int64_t el, i = 0; i < n; ++i) {
        std::cin >> el;
        reverse_hash[i] = ((i != 0 ? reverse_hash[i - 1] : 0) + el * powers[n - i - 1]) % mod;
        hash[i] = ((i != 0 ? hash[i - 1] : 0) + powers[i] * el) % mod;
    }

    for (ssize_t i = static_cast<ssize_t>(n - 1) / 2; i >= 0; --i) {
        if (reverse_hash[i] == substring_hash(hash, i + 1, 2 * i + 1, powers)) {
            std::cout << n - i - 1 << ' ';
        }
    }
    std::cout << n << '\n';

}