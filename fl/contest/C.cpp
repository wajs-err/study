#include <iostream>
#include <vector>


std::string check(const std::vector<int>& p, int m) {

    if (p[0] > 0) {
        return "";
    }
    for (size_t i = 1; i < p.size(); ++i) {
        if (p[i] > i || p[i] > p[i - 1] + 1) {
            return "";
        }
    }

    std::string s = "a";
    s.reserve(p.size());
    std::vector<int> used;
    int count = 1;
    int index;
    for (size_t i = 1; i < p.size(); ++i) {
        if (p[i] > 0) {
            s.push_back(s[p[i] - 1]);
            index = p[i - 1];
            while (s[index] != s[i] && index > 0) {
                index = p[index - 1];
            }
            if (s[index] == s[i] && p[i] != index + 1 || s[index] != s[i]) {
                return "";
            }
        } else {
            used.assign(count, 0);
            index = p[i - 1];
            while (index > 0) {
                used[s[index] - 'a'] = 1;
                index = p[index - 1];
            }
            used[0] = 1;
            s.push_back('0');
            for (int j = 0; j < used.size(); ++j) {
                if (used[j] == 0) {
                    s[i] = 'a' + j;
                    break;
                }
            }
            if (s.back() == '0') {
                s[i] = 'a' + count;
                ++count;
            }
        }
    }

    if (count > m) {
        return "";
    }

    return s;

}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n, m;
    std::vector<int> p;
    std::string s;

    while (std::cin >> n >> m) {

        p.resize(n);
        for (auto& el : p) {
            std::cin >> el;
        }

        s = check(p, m);
        if (!s.empty()) {
            std::cout << "YES\n" << s << '\n';
        } else {
            std::cout << "NO\n";
        }

    }

}