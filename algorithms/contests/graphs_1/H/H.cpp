#include <iostream>
#include <vector>


using std::vector;


class Graph {

public:

    enum color {
        WHITE = 1, GREY = 2, BLACK = 3
    };

    Graph() = delete;
    explicit Graph(int V): V(V), edges(std::vector<std::vector<std::pair<int/*vertex*/, int/*number*/>>>(V + 1)) {}
    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    ~Graph() = default;

    // adds edge (v, u) to the graph
    void push_edge(int v, int u, int i) {
        edges[v].emplace_back(u, i);
    }

public:

    int V;
    vector<vector<std::pair<int/*vertex*/, int/*number*/>>> edges;

public:

    void dfs(vector<bool>& used, vector<int>& ret, vector<int>& t_in, vector<int>& t_out,
             int& timer, int v, vector<int>& res, int p = -1) {

        used[v] = true;
        t_in[v] = timer++;
        ret[v] = t_in[v];

        for (auto& [to, n] : edges[v]) {

            if (to == p) {
                continue;
            }

            if (used[to]) {
                ret[v] = std::min(ret[v], t_in[to]);
                continue;
            }

            dfs(used, ret, t_in, t_out, timer, to, res, v);
            ret[v] = std::min(ret[v], ret[to]);

            if (ret[to] == t_in[to]) {
                res.push_back(n);
            }

        }

        t_out[v] = timer++;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int V, E;
    std::cin >> V >> E;

    Graph g(V);

    int v, u;
    for (int i = 0; i < E; ++i) {
        std::cin >> v >> u;
        g.push_edge(v, u, i + 1);
        g.push_edge(u, v, i + 1);
    }

    vector<bool> used(V + 1, false);
    vector<int> ret(V + 1);
    vector<int> t_in(V + 1);
    vector<int> t_out(V + 1);
    vector<int> res;
    int timer = 0;
    for (int i = 1; i <= V; ++i) {
        if (!used[i]) {
            g.dfs(used, ret, t_in, t_out, timer, i, res);
        }
    }

    std::sort(res.begin(), res.end());
    auto last = std::unique(res.begin(), res.end());
    res.erase(last, res.end());

    std::cout << res.size() << '\n';
    for (auto& a : res) {
        std::cout << a << ' ';
    }

}