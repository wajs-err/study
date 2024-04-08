#include <iostream>
#include <vector>
#include <unordered_set>


constexpr size_t size = 8000;
constexpr int64_t mod = 1'000'000'000 + 99'999;


std::vector<int64_t> powers(size);


void init() {
    for (int64_t p = 1; auto& el: powers) {
        el = p;
        (p *= 53) %= mod;
    }
}


std::string rand_string() {
    std::string s;
    s.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        s += rand() % 26 + 'a';
    }
    return s;
}


int64_t substring_hash(const std::vector<int64_t>& hash, size_t l, size_t r, const std::vector<int64_t>& powers) {
    return (mod + hash[r] - (l != 0 ? hash[l - 1] : 0)) * powers[hash.size() - r - 1] % mod;
}


size_t prefix_function_max(const std::string& s) {

    std::vector<int> p(s.size(), 0);

    int index;
    for (size_t i = 1; i < p.size(); ++i) {
        index = p[i - 1];
        while (s[index] != s[i] && index > 0) {
            index = p[index - 1];
        }
        if (s[index] == s[i]) {
            p[i] = index + 1;
        }
    }

    return *std::max_element(p.begin(), p.end());

}


std::vector<int> v_stoopid;
size_t stoopid(const std::string& s) {

    std::vector<int64_t> hash(s.size());
    hash[0] = s[0] - 'A' + 1;
    for (size_t i = 1; i < s.size(); ++i) {
        hash[i] = (hash[i - 1] + powers[i] * (s[i] - 'A' + 1)) % mod;
    }

    std::unordered_set<int64_t> hashes;
    size_t length = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        hashes.clear();
        hashes.reserve(s.size());
        for (size_t j = 0; j + i < s.size(); ++j) {
            if (hashes.insert(substring_hash(hash, j, j + i, powers)).second) {
                v_stoopid.push_back(i + 1);
                length += i + 1;
            }
        }
    }
    return length;
}


std::vector<size_t> v_smart;
size_t smart(const std::string& s) {
    size_t length = 0;
    std::string t;
    t.reserve(s.size());
    for (size_t suffix, i = 0; i < s.size(); ++i) {
        t += s[i];
        std::reverse(t.begin(), t.end());
        suffix = prefix_function_max(t);
        for (size_t j = 0; j < t.size() - suffix; ++j) {
            length += t.size() - j;
            v_smart.push_back(t.size() - j);
        }
        std::reverse(t.begin(), t.end());
    }
    return length;
}


int main() {

    std::cout << rand_string() << '\n';
    return 0;

    init();
    std::string s = "vzrccrhcgoerhtnzgdiaaqrhwxcbvreqrvsvobxvqboxwbyehi";

    for (size_t i = 0; ; ++i) {

        std::cout << i << ": ";

        if (stoopid(s) != smart(s)) {
//            std::cout << '\n' << stoopid(s) << '\n' << smart(s) << '\n';
            std::cout << s << '\n';
            std::sort(v_stoopid.begin(), v_stoopid.end());
            std::sort(v_smart.begin(), v_smart.end());
            for (size_t i = 1; i <= v_smart.back(); ++i) {
                std::cout << i << ": " <<
                    std::count_if(v_stoopid.begin(), v_stoopid.end(), [i](const auto& x) { return x == i; }) << ' ' <<
                    std::count_if(v_smart.begin(), v_smart.end(), [i](const auto& x) { return x == i; }) << '\n';
            }
            break;
        }

        std::cout << '\n';

    }

    std::cout << '\n';

}