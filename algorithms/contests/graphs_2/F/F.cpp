#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <numeric>


using std::vector;
using std::queue;
using std::set;
using std::pair;


class Graph {

private:

    const int INFINITY = std::numeric_limits<int>::max() / 2 - 1;

    size_t size;
    vector<vector<pair<int/*vertex*/, int/*weight*/>>> g;
    vector<int> dist;

public:

    Graph() = delete;
    Graph(size_t size): size(size), g(vector<vector<pair<int, int>>>(size)), dist(vector<int>(size, INFINITY)) {}
    ~Graph() = default;

    void push_edge(int u, int v, int weight) {
        g[u].emplace_back(v, weight);
        g[v].emplace_back(u, weight);
    }

    int Dijkstra(int s, int t) {

        dist[s] = 0;

        std::set<pair<int/*dist*/, size_t/*vertex*/>> unused;

        for (size_t i = 0; i < size; ++i) {
            unused.emplace(dist[i], i);
        }

        while (!unused.empty()) {
            auto [d, v] = *unused.begin();
            unused.erase(unused.begin());
            for (auto& [to, w] : g[v]) {
                if (unused.contains({dist[to], to}) && dist[to] >= dist[v] + w) {
                    unused.erase({dist[to], to});
                    dist[to] = dist[v] + w;
                    unused.emplace(dist[to], to);
                }
            }
        }

        return (dist[t] == INFINITY ? -1 : dist[t]);

    }

};


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    size_t n, m;
    std::cin >> n >> m;

    int s, t;
    std::cin >> s >> t;

    Graph g(n);

    int u, v, w;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> u >> v >> w;
        g.push_edge(u - 1, v - 1, w);
    }

    std::cout << g.Dijkstra(s - 1, t - 1) << '\n';

}