// This algorithm is probably O(E^3)
// But this problem can be solved in O(E^1.5)
// https://catalog.lib.kyushu-u.ac.jp/opac_download_md/14869/IJFCS-revision.pdf, pages 7-13
// This problem is in MST / matchings / flows contest but none of this is used here :)


#include <iostream>
#include <algorithm>
#include <vector>
#include <set>


using std::vector;
using std::pair;
using std::set;


class DirectedGraph {

private:

    struct Edge {

        size_t to;
        bool exists;

        Edge(size_t to, bool exists): to(to), exists(exists) {}

        ~Edge() = default;

    };

    size_t V;
    vector<vector<Edge>> adjacency_list;

    ssize_t dfs(size_t v, ssize_t max_outdegree, vector<bool>& used, vector<ssize_t>& outdegrees) {

        if (used[v]) {
            return -1;
        }
        used[v] = true;

        for (auto& edge : adjacency_list[v]) {

            if (used[edge.to] || !edge.exists) {
                continue;
            }

            if (outdegrees[edge.to] <= max_outdegree - 2) {
                edge.exists = false;
                for (auto& e : adjacency_list[edge.to]) {
                    if (e.to == v) {
                        e.exists = true;
                        break;
                    }
                }
                return static_cast<ssize_t>(edge.to);
            }

            ssize_t u = dfs(edge.to, max_outdegree, used, outdegrees);
            if (u != -1) {
                edge.exists = false;
                for (auto& e : adjacency_list[edge.to]) {
                    if (e.to == v) {
                        e.exists = true;
                        break;
                    }
                }
                return u;
            }

        }

        return -1;

    }

    vector<ssize_t> outdegrees() {

        vector<ssize_t> outdegrees(V, 0);

        for (size_t v = 0; v < V; ++v) {
            for (auto& edge : adjacency_list[v]) {
                if (edge.exists) {
                    ++outdegrees[v];
                }
            }
        }

        return outdegrees;

    }

    static ssize_t max_outdegree_vertex(const vector<ssize_t>& outdegrees) {
        return std::max_element(outdegrees.begin(), outdegrees.end()) - outdegrees.begin();
    }

public:

    explicit DirectedGraph(size_t V): V(V), adjacency_list(V) {}

    ~DirectedGraph() = default;

    void add_edge(size_t u, size_t v) {
        adjacency_list[u].emplace_back(v, true);
        adjacency_list[v].emplace_back(u, false);
    }

    // fins orientation of a graph such that maximum outdegree of vertex is minimal
    pair<size_t, set<pair<size_t, ssize_t>>> find_min_max_outdegree_orientation() {

        vector<ssize_t> outdegrees = this->outdegrees();
        vector<bool> used(V, false);

        ssize_t u = max_outdegree_vertex(outdegrees);
        ssize_t v = dfs(u, outdegrees[u], used, outdegrees);

        while (v != -1) {

            --outdegrees[u];
            ++outdegrees[static_cast<size_t>(v)];

            used.assign(V, false);

            u = max_outdegree_vertex(outdegrees);
            v = dfs(u, outdegrees[u], used, outdegrees);

        }

        set<pair<size_t, ssize_t>> orientation;

        for (size_t v = 0; v < V; ++v) {
            for (auto& edge : adjacency_list[v]) {
                if (edge.exists) {
                    orientation.emplace(v, edge.to);
                }
            }
        }

        return {outdegrees[max_outdegree_vertex(outdegrees)],
                orientation};

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V, E;
    std::cin >> V >> E;

    DirectedGraph G(V);

    vector<pair<size_t, size_t>> edges;

    size_t u, v;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> u >> v;
        edges.emplace_back(u - 1, v - 1);
        G.add_edge(u - 1, v - 1);
    }

    auto [d, orientation] = G.find_min_max_outdegree_orientation();

    std::cout << d << '\n';

    for (auto& edge : edges) {
        std::cout << !orientation.contains(edge) << ' ';
    }
    std::cout << '\n';

}