#include <iostream>
#include <vector>
#include <set>
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

    void dfs(vector<color>& colors, vector<int>& component, int curr,
             int v, std::set<std::pair<int, int>>& ret) {

        colors[v] = GREY;
        component[v] = curr;

        for (int& to : edges[v]) {

            if (colors[to] == BLACK && component[v] != component[to]) {
                ret.insert({component[v], component[to]});
            }

            if (colors[to] != WHITE) {
                continue;
            }

            dfs(colors, component, curr, to, ret);

        }

        colors[v] = BLACK;

    }

};


int main() {

    int V, E;
    std::cin >> V >> E;

    DAG g(V);

    int v, u;
    for (int i = 0; i < E; ++i) {
        std::cin >> v >> u;
        g.push_edge(v, u);
        g.push_edge(u, v);
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

    DAG gg(V);

    for (int i = 1; i <= V; ++i) {
        for (int j = 0; j < g.edges[i].size(); ++j) {
            gg.push_edge(g.edges[i][j], i);
        }
    }

    vector<DAG::color> colors_(V + 1, DAG::WHITE);
    vector<int> component(V + 1, -1);
    std::set<std::pair<int, int>> ret;
    int curr = 1;
    for (auto& [t, i] : q) {
        if (colors_[i] == DAG::WHITE) {
            gg.dfs(colors_, component, curr++, i, ret);
        }
    }

    std::cout << curr - 1 << '\n';
    for (size_t i = 1; i <= V; ++i) {
        std::cout << component[i] << ' ';
    }
    std::cout << '\n';

}