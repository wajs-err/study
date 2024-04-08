#include <iostream>
#include <vector>


using std::vector;


class DSU {

private:

    vector<size_t> parent;
    vector<size_t> size;

public:

    explicit DSU(size_t V): parent(V + 1, 0), size(V + 1, 1) {}

    ~DSU() = default;

    size_t get(size_t v) {

        if (parent[v] == 0) {
            return v;
        }

        return parent[v] = get(parent[v]);

    }

    void unite(size_t v, size_t u) {

        v = get(v);
        u = get(u);

        if (size[v] < size[u]) {
            std::swap(v, u);
        }

        // size[v] >= size[u]
        parent[u] = v;
        size[v] += size[u];

    }

};


class Graph {

private:

    struct Edge {

        size_t v;
        size_t u;
        size_t weight;

        Edge(size_t v, size_t u, size_t weight): v(v), u(u), weight(weight) {}

        ~Edge() = default;

    };

    size_t V;
    vector<Edge> edges;

public:

    explicit Graph(size_t V): V(V), edges() {}

    Graph(size_t V, size_t E): V(V), edges() {
        edges.reserve(E);
    }

    ~Graph() = default;

    void push_edge(size_t v, size_t u, size_t weight) {
        edges.emplace_back(v, u, weight);
    }

    size_t MST_Kruskal() {

        std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b){
            return a.weight < b.weight;
        });

        DSU dsu(V);

        size_t res = 0;

        for (auto& el : edges) {
            if (dsu.get(el.v) != dsu.get(el.u)) {
                dsu.unite(el.v, el.u);
                res += el.weight;
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

    std::cout << G.MST_Kruskal() << '\n';

}