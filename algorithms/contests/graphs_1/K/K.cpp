#include <iostream>
#include <vector>
#include <map>


using std::vector;


class Graph {

public:

    Graph() = delete;
    explicit Graph(int V): V(V), edges(vector<vector<int>>(V)) {}
    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    ~Graph() = default;

    // adds edge (v, u) to the graph
    void push_edge(int v, int u) {
        edges[v].push_back(u);
        edges[u].push_back(v);
    }

public:

    int V;
    vector<vector<int>> edges;

public:

    void dfs(vector<bool>& used, vector<int>& ret, vector<int>& t_in, vector<int>& t_out,
             int& timer, int v, int p = -1) {

        used[v] = true;
        t_in[v] = timer++;
        ret[v] = t_in[v];

        for (auto& to : edges[v]) {

            if (to == p) {
                continue;
            }

            if (used[to]) {
                ret[v] = std::min(ret[v], t_in[to]);
                continue;
            }

            dfs(used, ret, t_in, t_out, timer, to, v);
            ret[v] = std::min(ret[v], ret[to]);

        }

        t_out[v] = timer++;

    }

    void dfs(vector<vector<std::pair<int, int>>>& component, vector<bool>& used, vector<int>& ret,
             vector<int>& t_in, int v, int color, int& count, int p = -1) {

        used[v] = true;

        for (auto& to : edges[v]) {

            if (to == p) {
                continue;
            }

            if (used[to]) {
                if (t_in[to] < t_in[v]) {
                    component[color - 1].emplace_back(std::min(to, v), std::max(to, v));
                }
                continue;
            }

            if (ret[to] >= t_in[v]) {
                int new_color = ++count;
                component.emplace_back();
                component[new_color - 1].emplace_back(std::min(to, v), std::max(to, v));
                dfs(component, used, ret, t_in, to, new_color, count, v);
            } else {
                component[color - 1].emplace_back(std::min(to, v), std::max(to, v));
                dfs(component, used, ret, t_in, to, color, count, v);
            }

        }

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int V, E;
    std::cin >> V >> E;

    if (E == 0) {
        std::cout << 0 << '\n';
        return 0;
    }

    Graph g(V);

    std::map<std::pair<int/*from*/, int/*to*/>,
            std::vector<int>/*numbers*/> map;

    int v, u;
    for (int i = 0; i < E; ++i) {
        std::cin >> v >> u;
        map[{std::min(u, v), std::max(u, v)}].push_back(i);
    }

    for (auto& [pair, vec] : map) {
        g.push_edge(pair.first, pair.second);
    }

    vector<bool> used(V, false);
    vector<int> ret(V, -1);
    vector<int> t_in(V);
    vector<int> t_out(V);
    int timer = 0;
    for (int i = 0; i < V; ++i) {
        if (!used[i]) {
            g.dfs(used, ret, t_in, t_out, timer, i);
        }
    }

    vector<vector<std::pair<int, int>>> component;
    vector<bool> used__(V, false);
    int color = 0;
    for (int i = 0; i < V; ++i) {
        if (!used__[i]) {
            ++color;
            component.emplace_back();
            g.dfs(component, used__, ret, t_in, i, color, color, -1);
        }
    }

    size_t count = 0;
    for (auto& el : component) {
        if (!el.empty()) {
            ++count;
        }
    }

    std::cout << count << '\n';

    size_t size;
    for (size_t i = 1; i < component.size(); ++i) {

        size = 0;
        for (auto& pair : component[i]) {
            size += map[pair].size();
        }
        if (size == 0) {
            continue;
        }
        std::cout << size << ' ';

        for (auto& pair : component[i]) {
            for (auto& el : map[pair]) {
                std::cout << el << ' ';
            }
        }

        std::cout << '\n';

    }

}