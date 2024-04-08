#include <iostream>
#include <set>
#include <vector>


auto max(std::set<std::set<size_t>>& v) {

    if (v.begin()->empty()) {
        return v.begin();
    } else {
        auto it = v.end();
        return --it;
    }

}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t n, k, p;
    std::cin >> n >> k >> p;

    std::vector<std::set<size_t>> m(n + 1);
    std::set<std::set<size_t>> curr;
    std::vector<size_t> v(p);

    for (size_t i = 0; i < p; ++i) {
        std::cin >> v[i];
        m[v[i]].insert(i);
    }

    size_t res = 0;
    for (size_t i = 0; i < p; ++i) {

        if (curr.find(m[v[i]]) == curr.end()) {
            ++res;
            if (curr.size() == k) {
                m[v[i]].erase(i);
                curr.erase(max(curr));
                curr.insert(m[v[i]]);
            } else {
                curr.erase(m[v[i]]);
                m[v[i]].erase(i);
                curr.insert(m[v[i]]);
            }
        } else {
            curr.erase(curr.find(m[v[i]]));
            m[v[i]].erase(i);
            curr.insert(m[v[i]]);
        }

    }

    std::cout << res << '\n';

}