#include <iostream>
#include <string>
#include <vector>


std::vector<int> prefix_function(const std::string& s) {

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

    return p;

}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string s;
    std::string t;
    std::cin >> s >> t;

    int size = static_cast<int>(t.size());
    t += '#';
    t += s;

    for (size_t i = 0; const auto& el : prefix_function(t)) {
        if (el == size) {
            std::cout << i - 2 * size << ' ';
        }
        ++i;
    }
    std::cout << '\n';

}