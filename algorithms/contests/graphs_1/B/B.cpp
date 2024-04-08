#include <iostream>
#include <vector>


using std::vector;


vector<int> t_in;
vector<int> t_out;


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

    void find_paths(int v) {
        vector<color> colors(V + 1, WHITE);
        t_in.resize(V + 1);
        t_out.resize(V + 1);
        int timer = 0;
        dfs(colors, t_in, t_out, timer, v);
    }

    bool isParent(int u, int v) {
        return t_in[u] < t_in[v] && t_out[v] < t_out[u];
    }

private:

    int V;
    vector<vector<int>> edges;

    void dfs(vector<color>& colors, vector<int>& t_in, vector<int>& t_out, int& timer, int v, int p = -1) {

        t_in[v] = timer++;
        colors[v] = GREY;

        for (int& to : edges[v]) {

            if (colors[to] != WHITE) {
                continue;
            }

            dfs(colors, t_in, t_out, timer, to, v);

        }

        t_out[v] = timer++;
        colors[v] = BLACK;

    }

};


int main() {

    int V;
    std::cin >> V;

    DAG g(V);

    int u; int root;
    for (int i = 1; i <= V; ++i) {
        std::cin >> u;
        g.push_edge(u, i);
        if (u == 0) {
            root = i;
        }
    }

    g.find_paths(root);

    size_t count;
    std::cin >> count;

    int a, b;
    for (size_t i = 0; i < count; ++i) {

        std::cin >> a >> b;
        std::cout << g.isParent(a, b) << ' ';

    }

}