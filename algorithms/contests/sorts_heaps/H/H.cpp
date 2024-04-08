#include <iostream>
#include <vector>


void stable_counting_sort(std::vector<std::pair<uint8_t, unsigned long long>>& x) {

    std::vector< std::pair<uint8_t, unsigned long long> > y(x.size());

    unsigned int cnt[256];

    for (int i = 0; i < 256; ++i) {
        cnt[i] = 0;
    }

    for (auto &el : x) {
        ++cnt[el.first];
    }

    for (int i = 1; i < 256; ++i) {
        cnt[i] += cnt[i - 1];
    }

    for (int i = x.size() - 1; i >= 0; --i) {
        y[ --cnt[x[i].first] ] = x[i];
    }

    std::swap(x, y);

}


void LSD(std::vector<unsigned long long>& a) {

    for (int j = 0; j < 8; ++j) {

        std::vector< std::pair<uint8_t, unsigned long long> > p(a.size());

        for (int i = 0; i < a.size(); ++i) {
            p[i] = {(a[i] >> 8 * j) & 255, a[i]};
        }

        stable_counting_sort(p);

        for (int i = 0; i < a.size(); ++i) {
            a[i] = p[i].second;
        }

    }

}


int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    int n;
    std::cin >> n;

    std::vector<unsigned long long> a(n);

    for (auto &el : a) {
        std::cin >> el;
    }

    LSD(a);

    for (auto &el : a) {
        std::cout << el << ' ';
    }

}