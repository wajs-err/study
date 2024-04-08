#include <iostream>
#include <numeric>
#include <vector>
#include <set>


using std::vector;
using std::pair;
using std::set;


class Graph {

private:

    static constexpr size_t inf = std::numeric_limits<size_t>::max();

    struct Edge {

        size_t to;
        size_t weight;

        Edge(size_t to, size_t weight): to(to), weight(weight) {}

        ~Edge() = default;

    };

    size_t V;
    vector<vector<Edge>> edges;

public:

    explicit Graph(size_t V): V(V), edges(V + 1) {}

    Graph(size_t V, size_t E): V(V), edges() {
        edges.reserve(E);
    }

    ~Graph() = default;

    void push_edge(size_t v, size_t u, size_t weight) {
        edges[v].emplace_back(u, weight);
        edges[u].emplace_back(v, weight);
    }

    size_t MST_Prim() {

        if (V == 0) {
            return 0;
        }   // :)

        vector<size_t> d(V + 1, inf);
        set<pair<size_t/*weight*/, size_t/*vertex*/>> unused;

        unused.emplace(0, 1);
        d[1] = 0;
        for (size_t i = 2; i <= V; ++i) {
            unused.emplace(inf, i);
        }

        size_t res = 0;

        while (!unused.empty()) {

            auto min = *unused.begin();
            unused.erase(unused.begin());

            res += min.first;

            for (auto& edge : edges[min.second]) {
                if (unused.erase({d[edge.to], edge.to}) == 1) {
                    unused.emplace(std::min(d[edge.to], edge.weight), edge.to);
                    d[edge.to] = std::min(d[edge.to], edge.weight);
                }
            }

        }

        return res;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V, E;
    std::cin >> V >> E;

    Graph G(V);

    size_t v, u, weight;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> v >> u >> weight;
        G.push_edge(v, u, weight);
    }

    std::cout << G.MST_Prim() << '\n';

}