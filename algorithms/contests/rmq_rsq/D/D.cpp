#include <algorithm>
#include <iostream>
#include <vector>


class FenwickTree {

private:

    std::vector<int> v;

    static int f(int x) {
        return x & (x + 1);
    }

    static int g(int x) {
        return x | (x + 1);
    }

public:

    FenwickTree() = delete;
    explicit FenwickTree(size_t n) : v(n + 1) {}
    ~FenwickTree() = default;

    uint64_t sum(int x) {
        uint64_t res = 0;
        for (int i = x; i >= 0; i = f(i) - 1) {
            res += v[i];
        }
        return res;
    }

    void update(int x) {
        for (int i = x; i < v.size(); i = g(i)) {
            ++v[i];
        }
    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<std::pair<int, int>> v(n);
    std::vector<int> rights;

    for (auto& [l, r] : v) {
        std::cin >> l >> r;
        rights.push_back(r);
    }

    std::sort(v.begin(), v.end(), [](const auto& lhs, const auto& rhs){
        return lhs.first > rhs.first || lhs.first == rhs.first && lhs.second < rhs.second;
    });

    std::sort(rights.begin(), rights.end());
    rights.resize(std::unique(rights.begin(), rights.end()) - rights.begin());

    for (auto& [l, r] : v) {
        r = std::lower_bound(rights.begin(), rights.end(), r) - rights.begin();
    }

    FenwickTree f(n);
    uint64_t res = 0;

    uint64_t prev;
    for (int i = 0; i < n; ++i) {
        if (i == 0 || v[i] != v[i - 1]) {
            prev = f.get_count(v[i].second);
        }
        res += prev;
        f.update(v[i].second);
    }

    std::cout << res << '\n';

}