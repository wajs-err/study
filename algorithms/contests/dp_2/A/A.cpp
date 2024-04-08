// Problem D here is the same: https://codeforces.com/blog/entry/46031
// It's hard for me, because there is Aho-Corasick here, and I don't know it


#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>


using Matrix = std::vector<std::vector<int64_t>>;


constexpr int64_t minus_inf = std::numeric_limits<int64_t>::min() / 2;


void operator*=(Matrix& lhs, const Matrix& rhs) {

    auto size = lhs.size();

    Matrix answer(size, std::vector<int64_t>(size, minus_inf));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            for (size_t k = 0; k < size; ++k) {
                answer[i][j] = std::max(lhs[i][k] + rhs[k][j], answer[i][j]);
            }
        }
    }

    lhs = std::move(answer);

}


void power(Matrix& lhs, int64_t power) {

    auto size = lhs.size();

    Matrix M(size, std::vector<int64_t>(size, minus_inf));
    for (auto i = 0; i < size; ++i) {
        M[i][i] = 0;
    }

    while (power != 0) {

        if (power % 2 != 0) {
            M *= lhs;
        }

        lhs *= lhs;
        power >>= 1;

    }

    lhs = std::move(M);

}


int main() {

    size_t n;
    int64_t l;
    std::cin >> n >> l;

    std::vector<int64_t> w(n);
    for (auto& el : w) {
        std::cin >> el;
    }

    std::vector<std::string> strings(n);
    std::vector<std::string> prefixes;
    prefixes.emplace_back("");
    std::string tmp;
    for (auto& string : strings) {
        std::cin >> string;
        tmp.clear();
        for (const auto& ch : string) {
            tmp += ch;
            prefixes.push_back(tmp);
        }
    }

    std::sort(prefixes.begin(), prefixes.end(),
              [](const std::string& lhs, const std::string& rhs){
                  if (lhs.size() == rhs.size()) {
                      return lhs < rhs;
                  }
                  return lhs.size() < rhs.size();
              });
    prefixes.resize(std::unique(prefixes.begin(), prefixes.end()) - prefixes.begin());

    std::vector<int64_t> w_prefix(prefixes.size(), 0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < prefixes.size(); ++j) {
            if (prefixes[j].size() >= strings[i].size() &&
                    prefixes[j].substr(prefixes[j].size() - strings[i].size()) == strings[i]) {
                w_prefix[j] += w[i];
            }
        }
    }

    std::vector<std::vector<int64_t>> w_suffix(prefixes.size(), std::vector<int64_t>(26, 0));
    std::vector<std::vector<int64_t>> suffix(prefixes.size(), std::vector<int64_t>(26, 0));
    for (size_t i = 0; i < prefixes.size(); ++i) {
        tmp = prefixes[i];
        for (size_t j = 0; j < 26; ++j) {
            tmp += ('a' + j);
            for (ssize_t k = prefixes.size() - 1; k >= 0; --k) {
                if (tmp.size() >= prefixes[k].size() &&
                        tmp.substr(tmp.size() - prefixes[k].size()) == prefixes[k]) {
                    w_suffix[i][j] = w_prefix[k];
                    suffix[i][j] = k;
                    break;
                }
            }
            tmp.pop_back();
        }
    }

    Matrix mult(prefixes.size(), std::vector<int64_t>(prefixes.size(), minus_inf));
    for (size_t i = 0; i < prefixes.size(); ++i) {
        for (size_t j = 0; j < 26; ++j) {
            mult[i][suffix[i][j]] = w_suffix[i][j];
        }
    }

    power(mult, l);

    int64_t max = minus_inf;
    for (auto& el : mult[0]) {
        max = std::max(el, max);
    }

    std::cout << max << '\n';

}