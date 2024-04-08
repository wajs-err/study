#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


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

    auto p = prefix_function(t);
    std::cout << std::count_if(p.begin(), p.end(), [&size](const int& el){ return el == size; }) << '\n';
    for (size_t i = 0; const auto& el : p) {
        if (el == size) {
            std::cout << i - 2 * size + 1 << ' ';
        }
        ++i;
    }
    std::cout << '\n';

}
