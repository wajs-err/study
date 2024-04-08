#include <iostream>
#include <vector>
#include <algorithm>


using std::vector;


class BipartiteGraph {

    // Graph has two parts: L and R, |L| + |R| = V

private:

    size_t L;
    size_t R;
    vector<vector<size_t>> edges;   // edges from L to R only

    // helper function for Kuhn algorithm
    // finds augmenting path
    bool augment(size_t v, vector<bool>& used, vector<size_t>& matching) {

        if (used[v]) {
            return false;
        }
        used[v] = true;

        for (auto& to : edges[v]) {
            if (matching[to] == 0 || augment(matching[to], used, matching)) {
                matching[to] = v;
                return true;
            }
        }

        return false;

    }

public:

    BipartiteGraph(size_t L, size_t R): L(L), R(R), edges(L + 1) {}

    ~BipartiteGraph() = default;

    void add_edge(size_t u, size_t v) {
        edges[u].push_back(v);
    }

    size_t max_matching_Kuhn() {

        vector<bool> used(L + 1, false);
        vector<size_t> matching(R + 1, 0);

        for (size_t v = 1; v <= L; ++v) {
            if (augment(v, used, matching)) {
                used.assign(L + 1, false);
            }
        }

        size_t res = 0;
        for (size_t i = 1; i <= R; ++i) {
            if (matching[i] != 0) {
                ++res;
            }
        }

        return res;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t n, m;
    int64_t a, b;
    std::cin >> n >> m >> a >> b;

    vector<vector<size_t>> field(n, vector<size_t>(m));

    char c;
    size_t L = 0;
    size_t R = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            std::cin >> c;
            field[i][j] = c == '*' ? (i % 2 == j % 2 ? ++L : ++R) : 0;
        }
    }

    if (a >= 2 * b) {
        std::cout << static_cast<int64_t>(L + R) * b << '\n';
        return 0;
    }

    BipartiteGraph G(L, R);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {

            if (field[i][j] != 0 && i % 2 == j % 2) {

                if (i != 0 && field[i - 1][j] != 0) {
                    G.add_edge(field[i][j], field[i - 1][j]);
                }

                if (i != n - 1 && field[i + 1][j] != 0) {
                    G.add_edge(field[i][j], field[i + 1][j]);
                }

                if (j != 0 && field[i][j - 1] != 0) {
                    G.add_edge(field[i][j], field[i][j - 1]);
                }

                if (j != m - 1 && field[i][j + 1] != 0) {
                    G.add_edge(field[i][j], field[i][j + 1]);
                }

            }

        }
    }

    auto max_matching = static_cast<int64_t>(G.max_matching_Kuhn());
    std::cout << max_matching * a + (static_cast<int64_t>(L + R) - 2 * max_matching) * b << '\n';

}