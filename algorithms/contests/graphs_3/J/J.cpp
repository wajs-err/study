// This code is the slowest on Codeforces (it's 343 ms while the fastest one is 78 ms), so it can be improved
// DirectedMultigraph class probably can be removed

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>


using std::vector;
using std::pair;
using std::set;
using std::multiset;


class DirectedMultigraph {

private:

    size_t V;
    size_t E;
    vector<vector<pair<size_t/*vertex*/, size_t/*number*/>>> edges;

public:

    DirectedMultigraph(size_t V, const multiset<pair<size_t, size_t>>& edges_set)
            : V(V), E(edges_set.size()), edges(V) {
        size_t i = 0;
        for (auto it = edges_set.begin(); it != edges_set.end(); ++it, ++i) {
            edges[it->first].emplace_back(it->second, i);
        }
    }

    ~DirectedMultigraph() = default;

    bool dfs(size_t v, size_t t, vector<bool>& used, vector<size_t>& path) {

        for (auto& [to, n] : edges[v]) {

            if (used[n]) {
                continue;
            }
            used[n] = true;

            if (to == t) {
                path.push_back(to);
                return true;
            }

            if (!dfs(to, t, used, path)) {
                continue;
            }

            path.push_back(to);
            return true;

        }

        return false;

    }

    // just finds two different paths in a graph from s to t
    pair<vector<size_t>, vector<size_t>> find_two_paths(size_t s, size_t t) {

        vector<bool> used(E, false);
        vector<size_t> path_1;
        vector<size_t> path_2;

        dfs(s, t, used, path_1);
        path_1.push_back(s);
        std::reverse(path_1.begin(), path_1.end());

        dfs(s, t, used, path_2);
        path_2.push_back(s);
        std::reverse(path_2.begin(), path_2.end());

        return {path_1, path_2};

    }

};


class FlowNetwork {

private:

    struct Edge {

        size_t u;
        size_t v;
        mutable int capacity;
        mutable bool is_reverse;

        Edge(size_t u, size_t v): u(u), v(v), capacity(), is_reverse() {}
        Edge(size_t u, size_t v, int capacity, bool is_reverse)
                : u(u), v(v), capacity(capacity), is_reverse(is_reverse) {}
        ~Edge() = default;

        friend bool operator<(const Edge& lhs, const Edge& rhs) {
            return lhs.u < rhs.u || lhs.u == rhs.u && lhs.v < rhs.v;
        }

        friend bool operator==(const Edge& lhs, const Edge& rhs) {
            return lhs.u == rhs.u && lhs.v == rhs.v;
        }

    };

    size_t V;
    size_t s;
    size_t t;
    vector<vector<size_t>> adjacency_list;
    set<Edge> edges;

    bool dfs(size_t v, vector<bool>& used, vector<size_t>& path) {

        if (used[v]) {
            path.clear();
            return false;
        }
        used[v] = true;

        for (auto& to : adjacency_list[v]) {

            if (used[to]) {
                continue;
            }

            auto edge = edges.find({v, to});
            auto reverse_edge = edges.find({to, v});
            if (edge->capacity == 0) {
                continue;
            }
            edge->capacity -= 1;
            reverse_edge->capacity += 1;

            if (to == t) {
                path.clear();
                path.push_back(t);
                return true;
            }

            bool f = dfs(to, used, path);
            if (!f) {
                continue;
            }
            path.push_back(to);
            return true;

        }

        path.clear();
        return false;

    }

public:

    FlowNetwork(size_t V, size_t s, size_t t): V(V), s(s), t(t), adjacency_list(V), edges() {}

    ~FlowNetwork() = default;

    void add_edge(size_t u, size_t v) {

        auto it = edges.find(Edge{u, v});
        auto rit = edges.find(Edge{v, u});

        if (it == edges.end() && rit == edges.end()) {
            edges.emplace(u, v, 1, false);
            edges.emplace(v, u, 0, true);
            adjacency_list[u].push_back(v);
            adjacency_list[v].push_back(u);
            return;
        }

        it->capacity += 1;
        it->is_reverse = false;

    }

    // check Suurballe's algorithm (https://en.wikipedia.org/wiki/Suurballe%27s_algorithm)
    pair<bool, pair<vector<size_t>, vector<size_t>>> has_2_flow_Ford_Fulkerson() {

        vector<bool> used(V, false);
        vector<size_t> path_1;
        vector<size_t> path_2;

        bool flow_1 = dfs(s, used, path_1);
        path_1.push_back(s);
        std::reverse(path_1.begin(), path_1.end());

        used.assign(V, false);
        bool flow_2 = dfs(s, used, path_2);
        path_2.push_back(s);
        std::reverse(path_2.begin(), path_2.end());

        if (!flow_1 || !flow_2) {
            return {false, {{}, {}}};
        }

        multiset<pair<size_t, size_t>> paths;

        for (auto& path : {path_1, path_2}) {
            for (size_t i = 0; i < path.size() - 1; ++i) {
                auto it = paths.find({path[i + 1], path[i]});
                if (it != paths.end()) {
                    paths.erase(it);
                    continue;
                }
                paths.emplace(path[i], path[i + 1]);
            }
        }

        DirectedMultigraph G(V, paths);

        return {true, G.find_two_paths(s, t)};

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V, E;
    size_t s, t;
    std::cin >> V >> E >> s >> t;

    FlowNetwork G(V, s - 1, t - 1);

    size_t u, v;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> u >> v;
        G.add_edge(u - 1, v - 1);
    }

    auto p = G.has_2_flow_Ford_Fulkerson();

    if (!p.first) {
        std::cout << "NO\n";
        return 0;
    }

    std::cout << "YES\n";

    for (auto& v : p.second.first) {
        std::cout << v + 1 << ' ';
    }
    std::cout << '\n';


    for (auto& v : p.second.second) {
        std::cout << v + 1 << ' ';
    }
    std::cout << '\n';

}