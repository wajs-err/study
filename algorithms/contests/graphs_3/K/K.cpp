// Minimum cost maximum flow
// Successive shortest path algorithm


#include <iostream>
#include <numeric>
#include <vector>
#include <set>
#include <cmath>


using std::vector;
using std::pair;
using std::set;


class WeightedFlowNetwork {

public:

    struct Edge {

        size_t from;
        size_t to;
        int64_t capacity;
        int64_t cost;
        int64_t true_cost;
        size_t reverse_edge_number;

        Edge(): from(), to(), capacity(), cost(), true_cost(), reverse_edge_number() {}
        Edge(size_t u, size_t v, int64_t capacity, int64_t cost)
                : from(u), to(v), capacity(capacity), cost(cost), true_cost(cost), reverse_edge_number() {}
        ~Edge() = default;

        Edge& operator=(const Edge& other) = default;

        friend bool operator<(const Edge& lhs, const Edge& rhs) {
            return lhs.from < rhs.from || lhs.from == rhs.from && lhs.to < rhs.to;
        }

        friend bool operator==(const Edge& lhs, const Edge& rhs) {
            return lhs.from == rhs.from && lhs.to == rhs.to;
        }

    };

    static constexpr auto inf = std::numeric_limits<int64_t>::max() / 2;

    size_t V;
    size_t s;
    size_t t;
    vector<vector<Edge>> adjacency_list;
    vector<vector<Edge>> reverse_adjacency_list; // vertexes leading to this one

    bool Dijkstra(size_t u, size_t v, vector<Edge*>& path, vector<int64_t>& potentials) {

        vector<Edge*> edges(V, nullptr);
        path.clear();

        potentials.assign(V, inf);
        potentials[u] = 0;

        set<pair<int64_t/*potentials*/, size_t/*vertex*/>> unused;

        for (size_t i = 0; i < V; ++i) {
            unused.emplace(potentials[i], i);
        }

        while (!unused.empty()) {
            auto [d, vertex] = *unused.begin();
            unused.erase(unused.begin());
            for (auto& edge : adjacency_list[vertex]) {
                if (edge.capacity == 0) {
                    continue;
                }
                auto it = unused.find({potentials[edge.to], edge.to});
                if (it != unused.end() && potentials[edge.to] > d + edge.cost) {
                    unused.erase(it);
                    potentials[edge.to] = d + edge.cost;
                    unused.emplace(potentials[edge.to], edge.to);
                    edges[edge.to] = &edge;
                }
            }
        }

        size_t vertex = v;
        while (vertex != u) {
            if (edges[vertex] == nullptr || edges[vertex]->capacity <= 0) {
                return false;
            }
            path.push_back(edges[vertex]);
            vertex = edges[vertex]->from;
        }

        std::reverse(path.begin(), path.end());

        return potentials[v] < inf;

    }

    void FordBellman(size_t v, vector<int64_t>& potentials) {

        vector<int64_t> prev(V, inf);
        potentials.assign(V, inf);
        prev[v] = 0;
        potentials[v] = 0;

        int64_t min_cost;
        for (size_t k = 0; k < V; ++k) {

            for (size_t i = 0; i < V; ++i) {
                min_cost = inf;
                for (auto& edge : reverse_adjacency_list[i]) {
                    if (prev[edge.from] != inf && edge.capacity != 0) {
                        min_cost = std::min(prev[edge.from] + edge.cost, min_cost);
                    }
                }
                potentials[i] = std::min(prev[i], min_cost);
            }

            prev = potentials;

        }

    }

    int64_t push_flow(vector<Edge*>& path) {

        int64_t min_capacity = inf;

        for (auto& edge : path) {
            min_capacity = std::min(edge->capacity, min_capacity);
        }

        int64_t cost = 0;

        for (auto& edge : path) {
            cost += edge->true_cost * min_capacity;
            edge->capacity -= min_capacity;
            adjacency_list[edge->to][edge->reverse_edge_number].capacity += min_capacity;
        }

        return cost;

    }

    void apply_potential(vector<int64_t>& potentials) {
        for (size_t i = 0; i < V; ++i) {
            for (auto& edge : adjacency_list[i]) {
                edge.cost += potentials[edge.from] - potentials[edge.to];
            }
        }
    }

public:

    WeightedFlowNetwork(size_t V, size_t s, size_t t)
            : V(V), s(s), t(t), adjacency_list(V), reverse_adjacency_list(V) {}

    ~WeightedFlowNetwork() = default;

    void add_edge(size_t u, size_t v, int64_t capacity, int64_t cost) {

        adjacency_list[u].emplace_back(u, v, capacity, cost);
        adjacency_list[v].emplace_back(v, u, 0, -cost);

        adjacency_list[u].back().reverse_edge_number = adjacency_list[v].size() - 1;
        adjacency_list[v].back().reverse_edge_number = adjacency_list[u].size() - 1;

        reverse_adjacency_list[v].emplace_back(u, v, capacity, cost);
        reverse_adjacency_list[u].emplace_back(v, u, 0, -cost); // useless

    }

    int64_t min_cost_max_flow() {

        vector<int64_t> potentials;

        vector<Edge*> path;
        int64_t cost = 0;

        FordBellman(s, potentials);
        apply_potential(potentials);

        while (Dijkstra(s, t, path, potentials)) {
            cost += push_flow(path);
            apply_potential(potentials);
        }

        return cost;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V, E;
    std::cin >> V >> E;

    WeightedFlowNetwork G(V, 0, V - 1);

    size_t u, v;
    int64_t capacity, cost;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> u >> v >> capacity >> cost;
        G.add_edge(u - 1, v - 1, capacity, cost);
    }

    std::cout << G.min_cost_max_flow() << '\n';

}