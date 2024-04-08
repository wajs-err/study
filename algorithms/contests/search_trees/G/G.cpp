#include <iostream>
#include <vector>
#include <set>
#include <map>


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    struct S {
        int x;
        int y;
        bool is_end;
    };

    int x, y, n;
    std::cin >> n >> y >> x;

    std::vector<S> v(2 * n);

    for (size_t i = 0; i < 2 * n; i += 2) {
        std::cin >> v[i].x >> v[i].y >> v[i + 1].y;
        ++v[i + 1].y;
        v[i + 1].x = v[i].x;
        v[i].is_end = false;
        v[i + 1].is_end = true;
    }

    std::sort(v.begin(), v.end(), [](const S &lhs, const S &rhs) {
        return lhs.y < rhs.y;
    });

    std::map<int, std::multiset<int>::const_iterator> map;
    std::multiset<int> s;

    std::vector<int> opened(x, 0);

    s.insert(x);

    map[0] = s.begin();
    map[x] = s.end();

    size_t i = 0;
    for (int j = 0; j <= y; ++j) {

        while (v[i].y == j) {

            auto el = v[i];
            ++i;

            if (el.is_end) {
                auto curr = map.find(el.x);
                --opened[el.x];
                if (opened[el.x] != 0) {
                    continue;
                }
                auto l = *curr->second;
                s.erase(curr->second);
                auto prev = --map.erase(curr);
                l += *prev->second;
                s.erase(prev->second);
                map[prev->first] = s.insert(l);
                continue;
            }

            ++opened[el.x];
            if (opened[el.x] != 1) {
                continue;
            }
            map[el.x] = s.end();
            auto prev = --map.find(el.x);
            auto l = *prev->second;
            s.erase(prev->second);
            map[prev->first] = s.insert(el.x - prev->first);
            map[el.x] = s.insert(l - *map[prev->first]);

        }

        std::cout << *--s.end() << '\n';

    }

}