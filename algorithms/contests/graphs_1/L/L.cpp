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


struct Lamp {
    bool isOn;
    vector<size_t> s;
};


int main() {

    int n, m;
    std::cin >> n >> m;

    std::vector<Lamp> v(n);

    for (auto& el : v) {
        std::cin >> el.isOn;
    }

    size_t count;
    size_t x;
    for (size_t i = 0; i < m; ++i) {

        std::cin >> count;
        for (size_t j = 0; j < count; ++j) {
            std::cin >> x;
            v[x - 1].s.push_back(i);
        }

    }

    // a || b = (!a -> b) && (!b -> a)
    // a xor b = (!a || !b) && (a || b) = (a -> !b) && (b -> !a) && (!a -> b) && (!b -> a)
    // a !xor b = (a || !b) && (!a || b) = (!a -> !b) && (b -> a) && (a -> b) && (!b -> !a)

    DAG g(2 * m);

    for (size_t i = 0; i < n; ++i) {

        if (v[i].isOn) {
            // a !xor b
            // a = v[i].s[0]; b = v[i].s[1]; !a = v[i].s[0] + m; !b = v[i].s[1] + m
            g.push_edge(v[i].s[0], v[i].s[1]);          // a -> b
            g.push_edge(v[i].s[1], v[i].s[0]);          // b -> a
            g.push_edge(v[i].s[0] + m, v[i].s[1] + m);  // !a -> !b
            g.push_edge(v[i].s[1] + m, v[i].s[0] + m);  // !b -> !a
        } else {
            // a xor b
            // a = v[i].s[0]; b = v[i].s[1]; !a = v[i].s[0] + m; !b = v[i].s[1] + m
            g.push_edge(v[i].s[0], v[i].s[1] + m);      // a -> !b
            g.push_edge(v[i].s[1], v[i].s[0] + m);      // b -> !a
            g.push_edge(v[i].s[0] + m, v[i].s[1]);      // !a -> b
            g.push_edge(v[i].s[1] + m, v[i].s[0]);      // !b -> a
        }

    }

    int V = 2 * m;

    vector<DAG::color> colors(V, DAG::WHITE);
    vector<int> t_in(V);
    vector<int> t_out(V);
    int timer = 0;
    for (int i = 0; i < V; ++i) {
        if (colors[i] == DAG::WHITE) {
            g.dfs(colors, t_in, t_out, timer, i);
        }
    }

    vector<std::pair<int, int>> q(V);

    for (int i = 0; i < V; ++i) {
        q[i].first = t_out[i];
        q[i].second = i;
    }

    std::sort(q.rbegin(), q.rend(), [](std::pair<int, int>& lhs, std::pair<int, int>& rhs){
        return lhs.first < rhs.first;
    });

    DAG gg(V);

    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < g.edges[i].size(); ++j) {
            gg.push_edge(g.edges[i][j], i);
        }
    }

    vector<DAG::color> colors_(V, DAG::WHITE);
    vector<int> component(V, -1);
    std::set<std::pair<int, int>> ret;
    int curr = 0;
    for (auto& [t, i] : q) {
        if (colors_[i] == DAG::WHITE) {
            gg.dfs(colors_, component, curr++, i, ret);
        }
    }

    for (size_t i = 0; i < m; ++i) {
        if (component[i] == component[i + m]) {
            std::cout << "NO\n";
            return 0;
        }
    }

    std::cout << "YES\n";

}