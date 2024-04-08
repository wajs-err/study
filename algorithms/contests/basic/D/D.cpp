#include <iostream>
#include <queue>
#include <deque>

int main()
{

    std::queue <int> r;
    std::deque <int> l;

    size_t n;
    std::cin >> n;

    std::string s;
    int i;

    for (size_t j = 0; j < n; ++j) {

        std::cin >> s;

        if (s == "-") {

            if (!r.empty()) {

                std::cout << r.front() << '\n';
                r.pop();

                if (l.size() > r.size()) {
                    r.push(l.front());
                    l.pop_front();
                }

            } else {

                r.push(l.front());
                l.pop_front();
                std::cout << r.front() << '\n';

            }

        } else if (s == "+") {

            std::cin >> i;
            l.push_back(i);

            if (l.size() > r.size()) {
                r.push(l.front());
                l.pop_front();
            }

        } else if (s == "*") {

            std::cin >> i;
            if (l.size() < r.size())
                l.push_front(i);
            else
                r.push(i);

        }

    }

}