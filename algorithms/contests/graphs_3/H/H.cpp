#include <iostream>
#include <numeric>
#include <queue>
#include <vector>


using std::vector;
using std::queue;
using std::pair;


class Graph {

private:

    struct Edge {

        size_t from;
        size_t to;
        double capacity;
        size_t reverse_edge_number;

        Edge(): from(), to(), capacity(), reverse_edge_number() {}
        Edge(size_t u, size_t v, double capacity): from(u), to(v), capacity(capacity), reverse_edge_number() {}
        ~Edge() = default;

        Edge& operator=(const Edge& other) = default;

        friend bool operator<(const Edge& lhs, const Edge& rhs) {
            return lhs.from < rhs.from || lhs.from == rhs.from && lhs.to < rhs.to;
        }

        friend bool operator==(const Edge& lhs, const Edge& rhs) {
            return lhs.from == rhs.from && lhs.to == rhs.to;
        }

    };

    static constexpr auto size_t_inf = std::numeric_limits<size_t>::max() / 2;
    static constexpr auto int64_t_inf = std::numeric_limits<int64_t>::max() / 2;
    static constexpr auto double_inf = std::numeric_limits<double>::max() / 2;

    size_t n;   // number of given vertexes
    size_t V;   // number of vertexes in use
    size_t s;
    size_t t;
    size_t m;   // number of given edges
    vector<vector<Edge>> adjacency_list;
    vector<size_t> degrees;

    void add_edge(size_t u, size_t v, double capacity) {

        add_edge(u, v);
        --m;
        --degrees[u];
        --degrees[v];

        adjacency_list[u].back().capacity = capacity;
        adjacency_list[v].back().capacity = 0;

    }

    void max_flow_Dinic() {

        double max_flow = 0;

        while (true) {

            vector<size_t> level(V, size_t_inf);

            // construct layered network
            construct_layered_network(s, level);

            // find blocking flow
            vector<size_t> ptr(V, 0);   // ptr[i] is the number of first 'interesting' edge in adjacency_list[i]
            double flow = push_blocking_flow(s, double_inf, level, ptr);
            max_flow += flow;
            if (std::abs(flow) < 1e-9) {
                return;
            } // TODO

        }

    }

    void min_cut(vector<size_t>& S, vector<size_t>& T) {

        S.clear();
        T.clear();

        max_flow_Dinic();

        vector<bool> used(V, false);
        mark_reachable_vertexes(s, used);

        for (size_t i = 0; i < V; ++i) {
            if (used[i]) {
                S.push_back(i);
            } else {
                T.push_back(i);
            }
//            (used[i] ? S : T).push_back(i);       nice!
        }

    }

    // dfs
    void mark_reachable_vertexes(size_t v, vector<bool>& marked) /*NOLINT*/ {

        if (marked[v]) {
            return;
        }
        marked[v] = true;

        for (auto& edge : adjacency_list[v]) {
            if (marked[edge.to] || std::abs(edge.capacity) < 1e-9) { // TODO: double
                continue;
            }
            mark_reachable_vertexes(edge.to, marked);
        }

    }

    // bfs
    // (u, v).capacity = 1 <=> (u, v) in E
    void set_orientation(size_t v) {

        vector<bool> used(V, false);

        queue<size_t> q;
        q.push(v);

        size_t u;
        while (!q.empty()) {

            if (used[t]) {
                break;
            }

            u = q.front();
            q.pop();
            if (used[u]) {
                continue;
            }
            used[u] = true;

            for (auto& edge : adjacency_list[u]) {

                if (used[edge.to]) {
                    continue;
                }

                edge.capacity = 1;
                adjacency_list[edge.to][edge.reverse_edge_number].capacity = 1;
                q.push(edge.to);

            }

        }

    }

    // bfs
    void construct_layered_network(size_t v, vector<size_t>& level) {

        vector<bool> used(V, false);

        level[v] = 0;
        queue<size_t> q;
        q.push(v);

        size_t u;
        while (!q.empty()) {

            if (level[t] != size_t_inf) {
                return;
            }

            u = q.front();
            q.pop();

            for (auto& edge : adjacency_list[u]) {
                if (level[edge.to] == size_t_inf && std::abs(edge.capacity) >= 1e-9) { // TODO: double
                    level[edge.to] = level[edge.from] + 1;
                    q.push(edge.to);
                }
            }

        }

    }

    // dfs
    double push_blocking_flow(size_t v, double f, vector<size_t>& level, vector<size_t>& ptr) /*NOLINT*/ {

        if (v == t) {
            return f;
        }

        double x;
        while (ptr[v] < adjacency_list[v].size()) {

            auto& edge = adjacency_list[v][ptr[v]];

            if (level[edge.to] != level[edge.from] + 1 || std::abs(edge.capacity) < 1e-9) { // TODO: double
                ++ptr[v];
                continue;
            }

            x = push_blocking_flow(edge.to, std::min(f, edge.capacity), level, ptr);

            if (std::abs(x) < 1e-9) {
                ++ptr[v];
                continue;
            }

            edge.capacity -= x;
            adjacency_list[edge.to][edge.reverse_edge_number].capacity += x;
            return x;

        }

        return 0;

    }

public:

    explicit Graph(size_t n): n(n), V(n + 2), s(n), t(n + 1), m(), adjacency_list(V), degrees(V, 0) {}

    ~Graph() = default;

    void add_edge(size_t u, size_t v) {

        ++m;
        ++degrees[u];
        ++degrees[v];

        adjacency_list[u].emplace_back(u, v, 0);
        adjacency_list[v].emplace_back(v, u, 0);

        adjacency_list[u].back().reverse_edge_number = adjacency_list[v].size() - 1;
        adjacency_list[v].back().reverse_edge_number = adjacency_list[u].size() - 1;

    }

    vector<size_t> max_density_subgraph() {

        // adding source s and sink t. (s, a).capacity = 0 and (a, t).capacity = 0
        for (size_t i = 0; i < n; ++i) {
            add_edge(s, i, 0);
            add_edge(i, t, 0);
        }

        vector<size_t> H;   // set of max density subgraph vertexes
        vector<size_t> S;   // for min cut
        vector<size_t> T;   // for min cut
        double l = 0;
        auto u = static_cast<double>(m);

        double g;
        while (u - l >= 1 / static_cast<double>(n * (n - 1))) {

            g = (u + l) / 2;

            // construct flow network

            set_orientation(s);

            for (auto& edge : adjacency_list[s]) {
                edge.capacity = static_cast<double>(m);
                adjacency_list[edge.to][edge.reverse_edge_number].capacity = 0;
            }
            for (auto& edge : adjacency_list[t]) {
                edge.capacity = 0;
                adjacency_list[edge.to][edge.reverse_edge_number].capacity
                        = static_cast<double>(m) + 2 * g - static_cast<double>(degrees[edge.to]);
            }


            // find min cut
            min_cut(S, T);

            if (S.size() == 1) {
                u = g;
            } else {
                l = g;
                H = S;
            }

        }

        for (auto it = H.begin(); it != H.end(); ++it) {
            if (*it == s) {
                H.erase(it);
                break;
            }
        }

        return H;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V, E;
    std::cin >> V >> E;

    if (E == 0) {
        std::cout << "1\n1\n";
        return 0;
    }

    Graph G(V);

    size_t u, v;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> u >> v;
        G.add_edge(u - 1, v - 1);
    }

    auto H =  G.max_density_subgraph();

    std::cout << H.size() << '\n';

    for (auto& v : H) {
        std::cout << v + 1 << '\n';
    }

}