#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


int main()
{

    std::ifstream in("number.in");
    std::ofstream out("number.out");

    std::vector <std::string> s;

    while (in) {

        std::string s_;
        in >> s_;

        s.push_back(s_);

    }

    std::sort(s.begin(), s.end(), [](std::string s1, std::string s2) {
        if (s1 + s2 == s2 + s1)
            return s1.size() > s2.size();
        return (s1 + s2 > s2 + s1);
    });

    for (auto &el : s) {
        out << el;
    }

}