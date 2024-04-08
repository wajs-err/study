#include <algorithm>
#include <iostream>
#include <vector>
#include <set>


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int64_t l, n, k;
    std::cin >> l >> n >> k;

    int64_t s;
    s = l / k;

    std::vector<int64_t> v(n);
    std::multiset<int> set;
    std::vector<std::multiset<int>::iterator> vec(k);

    for (auto& el : vec) {
        el = set.emplace(0);
    }

    int64_t x;
    int tmp;
    for (size_t i = 0; i < n; ++i) {
        std::cin >> x;
        --x;
        v[i] = x;
        tmp = *vec[x / s];
        set.erase(vec[x / s]);
        vec[x / s] = set.emplace(tmp + 1);
    }

    std::sort(v.begin(), v.end(), [s](const auto& a, const auto& b){
        return a % s < b % s;
    });

    int min = *--set.end() - *set.begin();
    int64_t count = v[0] % s + 1;
    int64_t cut = l - 1;
    size_t j = 0;

    while (j < v.size()) {

        do {

            x = v[j];

            tmp = *vec[x / s];
            set.erase(vec[x / s]);
            vec[x / s] = set.emplace(tmp - 1);

            tmp = *vec[(k + x / s - 1) % k];
            set.erase(vec[(k + x / s - 1) % k]);
            vec[(k + x / s - 1) % k] = set.emplace(tmp + 1);

            ++j;

        } while (j < v.size() && v[j - 1] % s == v[j] % s);

        if (j >= v.size()) {
            break;
        }

        if (*--set.end() - *set.begin() == min) {
            count += v[j] % s - v[j - 1] % s;
        }

        if (*--set.end() - *set.begin() < min) {
            min = *--set.end() - *set.begin();
            count = v[j] % s - v[j - 1] % s;
            cut = v[j - 1];
        }

    }

    if (*--set.end() - *set.begin() == min) {
        count += s - 1 - v.back() % s;
    }

    std::cout << min << ' ' << count * k << '\n';
    std::cout << cut + 1 << '\n';

}