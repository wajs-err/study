#include <iostream>
#include <vector>
#include <algorithm>


using std::vector;


class DAG {

public:

    enum color {
        WHITE = 1, GREY = 2, BLACK = 3
    };

    DAG() = delete;
    explicit DAG(int V): V(V), edges(std::vector<std::vector<int>>(V + 1)) {}
    DAG(const DAG&) = delete;
    DAG& operator=(const DAG&) = delete;
    ~DAG() = default;

    // adds edge (v, u) to the graph
    void push_edge(int v, int u) {
        edges[v].push_back(u);
    }

private:

    int V;
    vector<vector<int>> edges;

public:

    vector<int> dfs(vector<color>& colors, int v) {

        colors[v] = GREY;

        for (int& to : edges[v]) {

            if (colors[to] == GREY) {
                vector<int> ret(2);
                ret[0] = to;
                ret[1] = v;
                return ret;
            }

            if (colors[to] != WHITE) {
                continue;
            }

            vector<int> get = dfs(colors, to);
            if (!get.empty()) {
                if (get[0] == get.back()) {
                    return get;
                } else {
                    get.push_back(v);
                    return get;
                }
            }

        }

        colors[v] = BLACK;

        return {};

    }

};


int check(vector<DAG::color>& colors, int start) {
    for (int i = start; i < colors.size(); ++i) {
        if (colors[i] == DAG::WHITE) {
            return i;
        }
    }
    return -1;
}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int V, E;
    std::cin >> V >> E;

    DAG g(V);

    int v, u;
    for (int i = 0; i < E; ++i) {
        std::cin >> v >> u;
        g.push_edge(v, u);
    }

    vector<DAG::color> colors(V + 1, DAG::WHITE);
    int start = check(colors, 1);
    while (start != -1) {

        vector<int> cycle = g.dfs(colors, start);

        if (!cycle.empty()) {
            std::cout << "YES\n";
            cycle.pop_back();
            std::reverse(cycle.begin(), cycle.end());
            for (int &el: cycle) {
                std::cout << el << ' ';
            }
            std::cout << '\n';
            return 0;
        }

        start = check(colors, start);

    }

    std::cout << "NO\n";

}