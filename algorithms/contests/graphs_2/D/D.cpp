#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <numeric>


using std::vector;
using std::queue;
using std::set;
using std::pair;


class DAG {

private:

    size_t V;
    vector<vector<pair<int/*vertex*/, long long/*weight*/>>> g; // g[v] is a vector of edges leading to v (edges (u, v))
    vector<vector<int>> g_;

    void dfs(size_t s, vector<bool>& used) {

        for (auto& to : g_[s]) {
            if (!used[to]) {
                dist[to] = minus_inf;
                used[to] = true;
                dfs(to, used);
            }
        }

    }

public:

    static constexpr long long inf = std::numeric_limits<long long>::max();
    static constexpr long long minus_inf = std::numeric_limits<long long>::min();

    vector<long long> dist;

    DAG() = delete;
    DAG(size_t V): V(V), g(vector<vector<pair<int, long long>>>(V)), g_(vector<vector<int>>(V)),
                   dist(vector<long long>(V, inf)) {}
    ~DAG() = default;

    void push_edge(int u, int v, long long weight) {
        g[v].emplace_back(u, weight);
        g_[u].push_back(v);
    }

    void FordBellman(int s) {

        vector<long long> prev(V, inf);
        vector<long long> curr(V, inf);
        prev[s] = 0;
        curr[s] = 0;

        for (size_t k = 1; k <= V; ++k) {

            for (size_t i = 0; i < V; ++i) {

                long long cost = inf;
                for (auto& [from, w] : g[i]) {
                    if (prev[from] != inf && prev[from] + w < cost) {
                        cost = prev[from] + w;
                    }
                }

                curr[i] = std::min(prev[i], cost);

            }

            if (k != V) {
                prev = curr;
            }

        }

        for (size_t i = 0; i < V; ++i) {
            dist[i] = curr[i];
        }

        vector<bool> used(V, false);
        for (size_t i = 0; i < V; ++i) {
            if (curr[i] < prev[i]) {
                dfs(i, used);
            }
        }

    }

};


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    size_t n, m;
    std::cin >> n >> m;

    int s;
    std::cin >> s;

    DAG g(n);

    int u, v;
    long long w;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> u >> v >> w;
        g.push_edge(u - 1, v - 1, w);
    }

    g.FordBellman(s - 1);

    for (auto& el : g.dist) {

        if (el == DAG::inf) {
            std::cout << "*\n";
            continue;
        }

        if (el == DAG::minus_inf) {
            std::cout << "-\n";
            continue;
        }

        std::cout << el << '\n';

    }

}