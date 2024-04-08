#include <iostream>
#include <stack>

int main()
{

    std::string s;
    std::cin >> s;

    std::stack <char> a;

    for (auto &ch : s) {

        if (ch == '(' || ch == '[' || ch == '{')
            a.push(ch);
        else if (!a.empty()) {

            if (ch == ')' && a.top() != '(' ||
                ch == ']' && a.top() != '[' ||
                ch == '}' && a.top() != '{') {

                std::cout << "no";
                return 0;

            } else
                a.pop();

        } else {

            std::cout << "no";
            return 0;

        }

    }

    if (a.empty())
        std::cout << "yes";
    else
        std::cout << "no";
}