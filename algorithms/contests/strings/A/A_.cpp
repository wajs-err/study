#include <iostream>
#include <string>
#include <vector>


std::vector<int> z_function(const std::string& s) {

    std::vector<int> z(s.size(), 0);

    int l = 0;
    int r = 0;
    for (int i = 1; i < z.size(); ++i) {

        if (i <= r) {
            z[i] = std::min(z[i - l], r - i + 1);
        }

        while (i + z[i] < z.size() && s[z[i]] == s[i + z[i]]) {
            ++z[i];
        }

        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }

    }

    return z;

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

    for (size_t i = 0; const auto& el : z_function(t)) {
        if (el == size) {
            std::cout << i - size - 1 << ' ';
        }
        ++i;
    }
    std::cout << '\n';

}