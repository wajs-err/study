// Assignment problem
// Solution with min cost max flow
// It passes all tests in > 1700ms (2s max), so it is cringe. Some solutions are 31 ms...


#include <iostream>
#include <numeric>
#include <vector>
#include <set>


using std::vector;
using std::pair;
using std::set;


// only for positive weights
class AssignmentProblemSolver {

private:

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

    };

    static constexpr auto inf = std::numeric_limits<int64_t>::max() / 2;

    size_t n;
    size_t V;
    size_t s;
    size_t t;
    vector<vector<Edge>> adjacency_list;

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

        return potentials[v] < inf;

    }

    int64_t push_flow(vector<Edge*>& path) {

        int64_t cost = 0;

        for (auto& edge : path) {
            cost += edge->true_cost;
            edge->capacity -= 1;
            adjacency_list[edge->to][edge->reverse_edge_number].capacity += 1;
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

    explicit AssignmentProblemSolver(size_t n): n(n), V(2 * n + 2), s(V - 2), t(V - 1), adjacency_list(V) {

        for (auto& list : adjacency_list) {
            list.reserve(n + 1);
        }

    }

    ~AssignmentProblemSolver() = default;

    void add_weight(size_t u, size_t v, int64_t capacity, int64_t cost) {

        adjacency_list[u].emplace_back(u, v, capacity, cost);
        adjacency_list[v].emplace_back(v, u, 0, -cost);

        adjacency_list[u].back().reverse_edge_number = adjacency_list[v].size() - 1;
        adjacency_list[v].back().reverse_edge_number = adjacency_list[u].size() - 1;

    }

    // this method can only be called after n^2 add_weight() calls
    int64_t solve() {

        // source and sink edges
        for (size_t i = 0; i < n; ++i) {
            add_weight(V - 2, i, 1, 0);
        }

        for (size_t i = 0; i < n; ++i) {
            add_weight(i + n, V - 1, 1, 0);
        }

        vector<int64_t> potentials;

        vector<Edge*> path;
        int64_t cost = 0;

        for (size_t i = 0; i < n; ++i) {
            Dijkstra(s, t, path, potentials);
            apply_potential(potentials);
            cost += push_flow(path);
        }

        return cost;

    }

    vector<pair<size_t, size_t>> get_solution() {

        vector<pair<size_t, size_t>> matching(n);

        for (size_t i = 0; i < n; ++i) {
            for (auto& edge : adjacency_list[i]) {
                if (edge.capacity == 0) {
                    matching[i] = {i, edge.to - n};
                    break;
                }
            }
        }

        return matching;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t n;
    std::cin >> n;

    AssignmentProblemSolver G(n);

    int64_t w;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            std::cin >> w;
            G.add_weight(i, j + n, 1, w);
        }
    }

    std::cout << G.solve() << '\n';

    for (auto& [i, j] : G.get_solution()) {
        std::cout << i + 1 << ' ' << j + 1 << '\n';
    }

}