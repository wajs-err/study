#include <iostream>

template <typename T>
class stack
{

private:

    size_t size_of = 0;

    struct node
    {

        T val;
        node *prev;

    };

    node *s = nullptr;

public:

    void push(T n)
    {

        node *new_el = new node();
        new_el->val = n;
        new_el->prev = s;

        s = new_el;

        ++size_of;

    }

    void pop()
    {

        node *el = s->prev;
        delete s;

        s = el;

        --size_of;

    }

    T top() {

        if (s != nullptr)
            return s->val;

    }

    void clear()
    {

        while (s != nullptr)
            pop();

        size_of = 0;

    }

    bool empty()
    {

        return s == nullptr;

    }

    size_t size() {

        return size_of;

    }

};


int main()
{

    stack <int> s;

    std::string str;

    while (true) {

        std::cin >> str;

        if (str == "push") {

            int x;
            std::cin >> x;
            s.push(x);
            std::cout << "ok\n";

        } else if (str == "clear") {

            s.clear();
            std::cout << "ok\n";

        } else if (str == "pop") {

            if (!s.empty()) {
                std::cout << s.top() << '\n';
                s.pop();
            } else
                std::cout << "error\n";

        } else if (str == "back") {

            if (!s.empty())
                std::cout << s.top() << '\n';
            else
                std::cout << "error\n";

        } else if (str == "size") {

            std::cout << s.size() << '\n';

        } else if (str == "exit") {

            std::cout << "bye\n";
            break;

        }

    }

}