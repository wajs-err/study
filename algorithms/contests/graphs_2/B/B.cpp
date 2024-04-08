#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_set>
#include <numeric>


using std::vector;
using std::queue;
using std::map;
using std::unordered_set;
using std::pair;


struct min {
    int operator()(int lhs, int rhs) {
        return std::min(lhs, rhs);
    }
};


class Graph {

private:

    size_t V;
    vector<vector<pair<int/*vertex*/, int/*weight*/>>> g;

public:

    static constexpr int inf = std::numeric_limits<int>::max() / 4;
    static constexpr int minus_inf = std::numeric_limits<int>::min();

    Graph() = delete;
    Graph(size_t V): V(V), g(vector<vector<pair<int, int>>>(V)) {}
    ~Graph() = default;

    void push_edge(int u, int v, int weight) {
        g[v].emplace_back(u, weight);
        g[u].emplace_back(v, weight);
    }

    int OK_bfs(int s, int t, int k) {

        vector<bool> used_s(V, false);
        vector<bool> used_t(V, false);
        vector<int> dist_s(V, inf);
        vector<int> dist_t(V, inf);
        vector<queue<int>> q_s(k + 1);
        vector<queue<int>> q_t(k + 1);

        q_s[0].push(s);
        q_t[0].push(t);
        dist_s[s] = 0;
        dist_t[t] = 0;

        int u;
        for (size_t j = 0; j < (V - 1) * k / 2 + 1; ++j) {

            while (!q_s[j % (k + 1)].empty()) {

                u = q_s[j % (k + 1)].front();
                q_s[j % (k + 1)].pop();

                if (used_t[u]) {
                    return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());
                }

                if (used_s[u]) {
                    continue;
                }
                used_s[u] = true;

                for (auto& [to, weight] : g[u]) {

                    if (weight < 0) {
                        continue;
                    }

                    if (dist_s[to] > dist_s[u] + weight) {
                        dist_s[to] = dist_s[u] + weight;
                        q_s[dist_s[to] % (k + 1)].push(to);
                    }

                }

            }

            while (!q_t[j % (k + 1)].empty()) {

                u = q_t[j % (k + 1)].front();
                q_t[j % (k + 1)].pop();

                if (used_s[u]) {
                    return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());
                }

                if (used_t[u]) {
                    continue;
                }
                used_t[u] = true;

                for (auto& [from, weight] : g[u]) {

                    if (weight < 0) {
                        continue;
                    }

                    if (dist_t[from] > dist_t[u] + weight) {
                        dist_t[from] = dist_t[u] + weight;
                        q_t[dist_t[from] % (k + 1)].push(from);
                    }

                }

            }

        }

        return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());

    }

    int find_min_cycle(int k) {

        unordered_set<int> set;

        int tmp;
        int m = inf;
        for (int i = 0; i < static_cast<int>(V); ++i) {
            for (auto& [to, weight] : g[i]) {

                // some hash magic (probably doesn't work)
                if (set.contains(std::min(i, to) * 1000 + std::max(i, to))) {
                    continue;
                }

                set.insert(std::min(i, to) * 1000 + std::max(i, to));

                for (auto& [t, w] : g[to]) {
                    if (t == i) {
                        w = -1;
                    }
                }

                tmp = weight;
                weight = -1;
                m = std::min(OK_bfs(i, to, k) + tmp, m);
                weight = tmp;

                for (auto& [t, w] : g[to]) {
                    if (w == -1) {
                        w = tmp;
                    }
                }

            }
        }

        return m;

    }

};


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    size_t n, m;
    std::cin >> n >> m;

    Graph g(n);

    int u, v, w;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> u >> v >> w;
        g.push_edge(u - 1, v - 1, w);
    }

    std::cout << g.find_min_cycle(10) << '\n';

}