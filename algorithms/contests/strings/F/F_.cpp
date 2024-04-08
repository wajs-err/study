#include <iostream>
#include <string>
#include <vector>


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

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string s;
    std::cin >> s;

    size_t length = 0;
    std::string t;
    t.reserve(s.size());
    for (size_t suffix, i = 0; i < s.size(); ++i) {
        t += s[i];
        std::reverse(t.begin(), t.end());
        suffix = prefix_function_max(t);
        for (size_t j = 0; j < t.size() - suffix; ++j) {
            length += t.size() - j;
        }
        std::reverse(t.begin(), t.end());
    }

    std::cout << length << '\n';

}