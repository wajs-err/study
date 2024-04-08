#include "deque.h"


int main() {

    Deque<int> deque;

    int r;
    for (size_t i = 0; i < 1'000'000'000; ++i) {

        r = rand() % 4;
        if (r == 0) {
            deque.push_back(r);
        } else if (r == 1) {
            deque.push_back(r);
        } else if (r == 2 && deque.size() != 0) {
            deque.pop_back();
        } else if (r == 3 && deque.size() != 0) {
            deque.pop_front();
        }

    }

    std::cout << "OK!\n";

}