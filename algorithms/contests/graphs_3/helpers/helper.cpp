#include <iostream>

int main() {

    srand(784123567);

    size_t V, E;
    V = 100;
    E = 1000;

    std::cout << V << ' ' << E << '\n';
    for (size_t i = 0; i < E; ++i) {
        std::cout << rand() % V + 1 << ' ' << rand() % V + 1 << ' ' <<
                rand() % 100 << ' ' << rand() % 200000 - 100000 << '\n';
    }

}