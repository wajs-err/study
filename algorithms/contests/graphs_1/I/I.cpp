#include <iostream>
#include <vector>
#include <algorithm>


using std::vector;


class Graph {

public:

    enum color {
        WHITE = 1, GREY = 2, BLACK = 3
    };

    Graph() = delete;
    explicit Graph(int V): V(V), edges(std::vector<std::vector<int>>(V + 1)) {}
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
             int& timer, int v, vector<int>& res, int p = -1) {

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

            dfs(used, ret, t_in, t_out, timer, to, res, v);
            ret[v] = std::min(ret[v], ret[to]);

            if (ret[to] >= t_in[v]) {
                res.push_back(v);
            }

        }

        t_out[v] = timer++;

    }

    int dfs(vector<bool>& used, vector<int>& ret, vector<int>& t_in, vector<int>& t_out,
            int& timer, int v) {

        int count = 0;

        used[v] = true;
        t_in[v] = timer++;
        ret[v] = t_in[v];

        for (auto& to : edges[v]) {

            if (used[to]) {
                continue;
            }

            ++count;
            dfs(used, ret, t_in, t_out, timer, to);

        }

        t_out[v] = timer++;

        return count;

    }

};


int main() {

    int V, E;
    std::cin >> V >> E;

    Graph g(V);

    int v, u;
    for (int i = 0; i < E; ++i) {
        std::cin >> v >> u;
        g.push_edge(v, u);
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

    vector<bool> used_(V + 1, false);
    vector<int> ret_(V + 1);
    vector<int> t_in_(V + 1);
    vector<int> t_out_(V + 1);
    int timer_ = 0;
    int count_ = g.dfs(used_, ret_, t_in_, t_out_, timer_, 1);

    std::sort(res.begin(), res.end());
    auto last = std::unique(res.begin(), res.end());
    res.erase(last, res.end());

    std::cout << (count_ >= 2 || res.empty() ? res.size() : res.size() - 1) << '\n';
    for (size_t i = (count_ >= 2 ? 0 : 1); i < res.size(); ++i) {
        std::cout << res[i] << ' ';
    }
    std::cout << '\n';

}