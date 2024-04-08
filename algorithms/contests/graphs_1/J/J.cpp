#include <iostream>
#include <vector>
#include <algorithm>


using std::vector;


class DAG {

public:

    DAG() = delete;
    explicit DAG(int V): V(V), edges(vector<vector<std::pair<int/*vertex*/, bool/*used*/>>>(V + 1)) {}
    DAG(const DAG&) = delete;
    DAG& operator=(const DAG&) = delete;
    ~DAG() = default;

    // adds edge (v, u) to the graph
    void push_edge(int v, int u) {
        edges[v].emplace_back(u, false);
    }

public:

    int V;
    vector<vector<std::pair<int/*vertex*/, bool/*used*/>>> edges;

public:

    void Euler(int v, vector<int>& r, vector<int>& first) {

        for (int i = first[v]; i < edges[v].size(); ++i) {

            ++first[v];
            if (!edges[v][i].second) {
                edges[v][i].second = true;
                Euler(edges[v][i].first, r, first);
            }

        }

        r.push_back(v);

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int V, A;
    std::cin >> V >> A;

    DAG g(V);

    bool b;
    for (int i = 1; i <= V; ++i) {
        for (int j = 1; j <= V; ++j) {
            std::cin >> b;
            if (!b && i != j) {
                g.push_edge(i, j);
            }
        }
    }

    vector<int> r;
    vector<int> first(V + 1);
    g.Euler(A, r, first);

    std::reverse(r.begin(), r.end());
    for (size_t i = 0; i < r.size() - 1; ++i) {
        std::cout << r[i] << ' ' << r[i + 1] << '\n';
    }

}