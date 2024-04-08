#include <set>
#include <map>
#include <iostream>
#include <vector>

int main() {

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    unsigned long long N;
    int M; int K;
    std::cin >> N >> M >> K;

    std::vector<std::set<unsigned long long>> v(M + 1);
    std::map<unsigned long long, std::set<int>> m;
    unsigned long long int e;
    int s;
    std::string str;

    for (int i = 0; i < K; ++i) {

        std::cin >> str;

        if (str[0] == 'A') {
            std::cin >> e;
            std::cin >> s;
            v[s].insert(e);
            m[e].insert(s);
            continue;
        }

        if (str[0] == 'D') {
            std::cin >> e;
            std::cin >> s;
            v[s].erase(e);
            m[e].erase(s);
            continue;
        }

        if (str[0] == 'C') {
            std::cin >> s;
            for (auto& el : v[s]) {
                m[el].erase(s);
            }
            v[s].clear();
            continue;
        }

        if (str == "LISTSET") {
            std::cin >> s;
            for (auto& el : v[s]) {
                std::cout << el << ' ';
            }
            if (v[s].empty()) {
                std::cout << -1;
            }
            std::cout << '\n';
            continue;
        }

        if (str == "LISTSETSOF") {
            std::cin >> e;
            for (auto& el : m[e]) {
                std::cout << el << ' ';
            }
            if (m[e].empty()) {
                std::cout << -1;
            }
            std::cout << '\n';
            continue;
        }

    }

}