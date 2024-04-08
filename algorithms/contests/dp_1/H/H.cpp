#include <iostream>
#include <vector>


void lcs(const std::string& s, const std::string& t, std::vector<int16_t>& res) {

    std::vector<int16_t> prev(t.size() + 1, 0);
    std::vector<int16_t> curr(t.size() + 1, 0);

    for (size_t i = 1; i <= s.size(); ++i) {
        for (size_t j = 1; j <= t.size(); ++j) {
            curr[j] = std::max(static_cast<int16_t>(s[i - 1] == t[j - 1] ? prev[j - 1] + 1 : 0),
                               std::max(prev[j], curr[j - 1]));
        }
        std::swap(prev, curr);
    }

    std::swap(res, prev);
    res.erase(res.begin());

}

/*
 https://neerc.ifmo.ru/wiki/index.php?title=%D0%97%D0%B0%D0%B4%D0%B0%D1%87%D0%B0_%D0%BE_%D0%BD%D0%B0%D0%B8%D0%B1%D0%BE%D0%BB%D1%8C%D1%88%D0%B5%D0%B9_%D0%BE%D0%B1%D1%89%D0%B5%D0%B9_%D0%BF%D0%BE%D0%B4%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D0%B8
 too much copying here...
*/
void Hirshberg(const std::string& s, std::string& t, std::string& w) {

    if (t.empty()) {
        return;
    }

    if (s.size() == 1) {
        if (t.find(s[0]) != std::string::npos) {
            w.push_back(s[0]);
        }
        return;
    }

    std::vector<int16_t> first;
    std::vector<int16_t> second;

    std::string a = s.substr(0, s.size() / 2);
    lcs(a, t, first);

    std::string b = s.substr(s.size() / 2, s.size());
    std::reverse(b.begin(), b.end());
    std::reverse(t.begin(), t.end());
    lcs(b, t, second);
    std::reverse(b.begin(), b.end());
    std::reverse(t.begin(), t.end());

    int16_t max = second.back();
    int16_t index = -1;

    for (int16_t i = 0; i < t.size() - 1; ++i) {
        if (first[i] + second[t.size() - 2 - i] > max) {
            max = first[i] + second[t.size() - 2 - i];
            index = i;
        }
    }

    if (first.back() > max) {
        index = static_cast<int16_t>(t.size() - 1);
    }

    std::string q = t.substr(0, index + 1);
    Hirshberg(a, q, w);
    q = t.substr(index + 1, t.size());
    Hirshberg(b, q, w);

}


int main() {

    std::string s;
    std::string t;
    std::string w;

    std::cin >> s >> t;

    Hirshberg(s, t, w);

    std::cout << w << '\n';

}