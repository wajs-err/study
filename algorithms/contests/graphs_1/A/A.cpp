#include <iostream>
#include <vector>


int main() {

    int V, E;
    std::cin >> V >> E;

    std::vector<std::vector<int>> m(V, std::vector<int>(E));

    int v, u;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> v >> u;
        m[v - 1][i] = 1;
        m[u - 1][i] = 1;
    }

    for (auto& vec : m) {
        for (auto& el : vec) {
            std::cout << el << ' ';
        }
        std::cout << '\n';
    }

}