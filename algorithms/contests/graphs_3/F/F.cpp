#include <iostream>
#include <vector>


using std::vector;
using std::pair;


// class BipartiteGraph can be removed,
// because DAG internal structure is same as BipartiteGraph internal structure
// so max_matching_Kuhn() can be called inside DAG
/*
class DAG {

private:

    size_t V;
    vector<vector<size_t>> edges;

    // helper function for Kuhn algorithm
    bool augment(size_t v, vector<bool>& used, vector<size_t>& matching) {

        if (used[v]) {
            return false;
        }
        used[v] = true;

        for (auto& to : edges[v]) {
            if (matching[to] == 0 || augment(matching[to], used, matching)) {
                matching[to] = v;
                return true;
            }
        }

        return false;

    }

    vector<size_t> max_matching_Kuhn() {

        vector<bool> used(V + 1, false);
        vector<size_t> matching(V + 1, 0);

        for (size_t v = 1; v <= V; ++v) {
            if (augment(v, used, matching)) {
                used.assign(V + 1, false);
            }
        }

        return matching;

    }


public:

    explicit DAG(size_t V): V(V), edges(V + 1) {}

    ~DAG() = default;

    void add_edge(size_t u, size_t v) {
        edges[u].push_back(v);
    }

    size_t max_path_cover() {

        auto matching = max_matching_Kuhn();

        vector<bool> used(V + 1, false);

        size_t res = 0;
        size_t v;
        bool f;

        for (size_t i = 1; i <= V; ++i) {

            if (used[i]) {
                continue;
            }

            f = true;
            v = i;
            while (matching[v] != 0) {
                v = matching[v];
                if (used[v]) {
                    f = false;
                    continue;
                }
                used[v] = true;
            }

            used[v] = true;

            if (f) {
                ++res;
            }

        }

        return res;

    }

};
*/


class BipartiteGraph {

    // Graph has two parts: L and R, |L| + |R| = V

private:

    size_t L;
    size_t R;
    vector<vector<size_t>> edges;   // edges from L to R only

    // helper function for Kuhn algorithm
    bool augment(size_t v, vector<bool>& used, vector<size_t>& matching) {

        if (used[v]) {
            return false;
        }
        used[v] = true;

        for (auto& to : edges[v]) {
            if (matching[to] == 0 || augment(matching[to], used, matching)) {
                matching[to] = v;
                return true;
            }
        }

        return false;

    }

public:

    BipartiteGraph(size_t L, size_t R): L(L), R(R), edges(L + 1) {}

    ~BipartiteGraph() = default;

    void add_edge(size_t u, size_t v) {
        edges[u].push_back(v);
    }

    vector<size_t> max_matching_Kuhn() {

        vector<bool> used(L + 1, false);
        vector<size_t> matching(R + 1, 0);

        for (size_t v = 1; v <= L; ++v) {
            if (augment(v, used, matching)) {
                used.assign(L + 1, false);
            }
        }

        return matching;

    }

};


class DAG {

private:

    size_t V;
    vector<vector<size_t>> edges;

    void dfs(size_t v, vector<bool>& used) {
        used[v] = true;
        for (auto& to : edges[v]) {
            if (!used[to]) {
                dfs(to, used);
            }
        }
    }

public:

    explicit DAG(size_t V): V(V), edges(V + 1) {}

    ~DAG() = default;

    void add_edge(size_t u, size_t v) {
        edges[u].push_back(v);
    }

    size_t max_path_cover() {

        BipartiteGraph H(V, V);

        for (size_t from = 1; from <= V; ++from) {
            for (auto& to : edges[from]) {
                H.add_edge(to, from);
            }
        }

        auto matching = H.max_matching_Kuhn();

        vector<bool> used(V + 1, false);

        size_t res = 0;
        size_t v;
        bool f;

        for (size_t i = 1; i <= V; ++i) {

            f = true;

            if (used[i]) {
                continue;
            }

            v = i;
            while (matching[v] != 0) {
                v = matching[v];
                if (used[v]) {
                    f = false;
                    continue;
                }
                used[v] = true;
            }

            used[v] = true;

            if (f) {
                ++res;
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

    DAG G(V);

    size_t u, v;
    for (size_t i = 0; i < E; ++i) {
        std::cin >> u >> v;
        G.add_edge(u, v);
    }

    std::cout << G.max_path_cover() << '\n';

}