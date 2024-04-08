#include <iostream>
#include <vector>
#include <algorithm>


using std::vector;


class DAG {

public:

    enum color {
        WHITE = 1, GREY = 2, BLACK = 3
    };

    DAG() = delete;
    explicit DAG(int V): V(V), edges(std::vector<std::vector<int>>(V + 1)) {}
    DAG(const DAG&) = delete;
    DAG& operator=(const DAG&) = delete;
    ~DAG() = default;

    // adds edge (v, u) to the graph
    void push_edge(int v, int u) {
        edges[v].push_back(u);
    }

public:

    int V;
    vector<vector<int>> edges;

public:

    void dfs(vector<color>& colors, vector<int>& t_in, vector<int>& t_out, int& timer, int v) {

        colors[v] = GREY;
        t_in[v] = timer++;

        for (int& to : edges[v]) {

            if (colors[to] != WHITE) {
                continue;
            }

            dfs(colors, t_in, t_out, timer, to);

        }

        colors[v] = BLACK;
        t_out[v] = timer++;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int V, E;
    std::cin >> V >> E;

    DAG g(V);

    int v, u;
    for (int i = 0; i < E; ++i) {
        std::cin >> v >> u;
        g.push_edge(v, u);
    }

    vector<DAG::color> colors(V + 1, DAG::WHITE);
    vector<int> t_in(V + 1);
    vector<int> t_out(V + 1);
    int timer = 0;
    for (int i = 1; i <= V; ++i) {
        if (colors[i] == DAG::WHITE) {
            g.dfs(colors, t_in, t_out, timer, i);
        }
    }

    vector<std::pair<int, int>> q(V);

    for (int i = 1; i <= V; ++i) {
        q[i - 1].first = t_out[i];
        q[i - 1].second = i;
    }

    std::sort(q.rbegin(), q.rend(), [](std::pair<int, int>& lhs, std::pair<int, int>& rhs){
        return lhs.first < rhs.first;
    });

    for (auto& [t, v] : q) {
        for (auto& to : g.edges[v]) {
            if (t_out[to] >= t) {
                std::cout << -1 << '\n';
                return 0;
            }
        }
    }

    for (auto& [t, n] : q) {
        std::cout << n << ' ';
    }

    std::cout << '\n';

}