// This solution is correct, but it gets TL for some reason.
// Both solutions are O(n^2), but this is 5 or more times slower...


#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>


int64_t substring_hash(const std::vector<int64_t>& hash, size_t l, size_t r,
                       const std::vector<int64_t>& powers, int64_t mod) {
    return (mod + hash[r] - (l != 0 ? hash[l - 1] : 0)) * powers[hash.size() - r - 1] % mod;
}


int main() {

    constexpr int64_t mod_1 = 1'000'000'000 + 7;
    constexpr int64_t mod_2 = 1'000'000'000 + 9;

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string s;
    std::cin >> s;

    std::vector<int64_t> powers_1(s.size());
    for (int64_t p = 1; auto& el : powers_1) {
        el = p;
        (p *= 53) %= mod_1;
    }
    std::vector<int64_t> powers_2(s.size());
    for (int64_t p = 1; auto& el : powers_2) {
        el = p;
        (p *= 53) %= mod_2;
    }

    std::vector<int64_t> hash_1(s.size());
    std::vector<int64_t> hash_2(s.size());
    hash_1[0] = s[0] - 'A' + 1;
    hash_2[0] = s[0] - 'A' + 1;
    for (size_t i = 1; i < s.size(); ++i) {
        hash_1[i] = (hash_1[i - 1] + powers_1[i] * (s[i] - 'A' + 1)) % mod_1;
        hash_2[i] = (hash_2[i - 1] + powers_2[i] * (s[i] - 'A' + 1)) % mod_2;
    }

    std::unordered_set<int64_t> hashes_1;
    std::unordered_set<int64_t> hashes_2;
    size_t length = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        hashes_1.clear();
        hashes_1.reserve(s.size());
        hashes_2.clear();
        hashes_2.reserve(s.size());
        for (size_t j = 0; j + i < s.size(); ++j) {
            if (!hashes_1.contains(substring_hash(hash_1, j, j + i, powers_1, mod_1)) ||
                !hashes_2.contains(substring_hash(hash_2, j, j + i, powers_2, mod_2))) {
                hashes_1.insert(substring_hash(hash_1, j, j + i, powers_1, mod_1));
                hashes_2.insert(substring_hash(hash_2, j, i + j, powers_2, mod_2));
                length += i + 1;
            }
        }
    }

    std::cout << length << '\n';

}