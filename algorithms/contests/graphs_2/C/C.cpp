#include <iostream>
#include <vector>
#include <queue>
#include <numeric>


using std::vector;
using std::queue;


class DAG {

private:

    const int inf = std::numeric_limits<int>::max() / 2 - 1;

    size_t size;
    vector<vector<int>> g;

public:

    DAG() = delete;
    explicit DAG(size_t size): size(size), g(vector<vector<int>>(size, vector<int>(size))) {}
    ~DAG() = default;

    void read() {
        for (auto& vec : g) {
            for (auto& el : vec) {
                std::cin >> el;
            }
        }
    }

    vector<vector<int>> Floyd() {

        vector<vector<int>> a(size, vector<int>(size));
        vector<vector<int>> b(size, vector<int>(size));

        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < g[i].size(); ++j) {
                b[i][j] = (g[i][j] == 0 ? inf : g[i][j]);
                if (i == j) {
                    b[i][j] = 0;
                }
            }
        }

        for (size_t k = 0; k < size; ++k) {
            for (size_t i = 0; i < size; ++i) {
                for (size_t j = 0; j < size; ++j) {
                    a[i][j] = std::min(b[i][j], b[i][k] + b[k][j]);
                }
            }
            b = a;
        }

        return b;

    }

};


int main() {

    int size;
    std::cin >> size;
    DAG g(size);
    g.read();

    for (auto& vec : g.Floyd()) {
        for (auto& el : vec) {
            std::cout << el << ' ';
        }
        std::cout << '\n';
    }

}