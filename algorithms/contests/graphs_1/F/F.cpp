#include <iostream>
#include <vector>
#include <set>


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

    void shrink_to_fit() {
        V = 0;
        edges.shrink_to_fit();
    }

public:

    int V;
    vector<vector<int>> edges;

public:

    void dfs(vector<color>& colors, vector<int>& t_in, vector<int>& t_out, int& timer, int v) {

        colors[v] = GREY;
        t_in[v] = timer++;

        for (int& to : edges[v]) {

            if (colors[to] != WHITE) {
                continue;
            }

            dfs(colors, t_in, t_out, timer, to);

        }

        colors[v] = BLACK;
        t_out[v] = timer++;

    }

    void dfs(vector<color>& colors, vector<int>& component, int curr,
             int v, std::set<std::pair<int, int>>& ret) {

        colors[v] = GREY;
        component[v] = curr;

        for (int& to : edges[v]) {

            if (colors[to] == BLACK && component[v] != component[to]) {
                ret.insert({component[to], component[v]});
            }

            if (colors[to] != WHITE) {
                continue;
            }

            dfs(colors, component, curr, to, ret);

        }

        colors[v] = BLACK;

    }

    void st(vector<bool>& used, int& w, bool& not_found, int x, std::set<int>& sinks) {

        if (!used[x]) {

            if (sinks.contains(x)) {
                w = x;
                not_found = false;
            }

            used[x] = true;

            for (auto& to : edges[x]) {
                if (not_found) {
                    st(used, w, not_found, to, sinks);
                }
            }

        }

    }

};


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


    // topological sort

    vector<DAG::color> colors(V + 1, DAG::WHITE);
    vector<int> t_in(V + 1);
    vector<int> t_out(V + 1);
    int timer = 0;
    for (int i = 1; i <= V; ++i) {
        if (colors[i] == DAG::WHITE) {
            g.dfs(colors, t_in, t_out, timer, i);
        }
    }

    colors.shrink_to_fit();
    t_in.shrink_to_fit();

    vector<std::pair<int, int>> q(V);

    for (int i = 1; i <= V; ++i) {
        q[i - 1].first = t_out[i];
        q[i - 1].second = i;
    }

    t_out.shrink_to_fit();

    std::sort(q.rbegin(), q.rend(), [](std::pair<int, int>& lhs, std::pair<int, int>& rhs){
        return lhs.first < rhs.first;
    });


    // condensation

    DAG gg(V); // graph with reversed edges

    for (int i = 1; i <= V; ++i) {
        for (int j = 0; j < g.edges[i].size(); ++j) {
            gg.push_edge(g.edges[i][j], i);
        }
    }

    vector<DAG::color> colors_(V + 1, DAG::WHITE);
    vector<int> component(V + 1, -1); // component[vertex] = number of its component
    std::set<std::pair<int, int>> edges; // set of edges between components (u, v) - from component u to component v
    int curr = 0;
    for (auto& [t, i] : q) {
        if (colors_[i] == DAG::WHITE) {
            gg.dfs(colors_, component, ++curr, i, edges);
        }
    }

    q.shrink_to_fit();
    colors_.shrink_to_fit();
    gg.shrink_to_fit();
    g.shrink_to_fit(); // +

    vector<int> encrypt(curr + 1);
    for (int i = 1; i < component.size(); ++i) {
        encrypt[component[i]] = i;
    }

    DAG condensation(curr);
    DAG nonDAG_condensation(curr);
    DAG reversed_condensation(curr);

    for (auto& [from, to] : edges) {
        condensation.push_edge(from, to);
        nonDAG_condensation.push_edge(from, to);
        nonDAG_condensation.push_edge(to, from);
        reversed_condensation.push_edge(to, from);
    }

    edges.clear();
    component.shrink_to_fit();


    // searching for isolated vertexes in condensation

    vector<int> isolated;

    for (int i = 1; i < nonDAG_condensation.edges.size(); ++i) {
        if (nonDAG_condensation.edges[i].empty()) {
            isolated.push_back(i);
        }
    }


    // searching for sources and sinks

    std::set<int> sources;
    std::set<int> sinks;

    for (int i = 1; i < reversed_condensation.edges.size(); ++i) {
        if (reversed_condensation.edges[i].empty() && !nonDAG_condensation.edges[i].empty()) {
            sources.insert(i);
        }
    }

    for (int i = 1; i < condensation.edges.size(); ++i) {
        if (condensation.edges[i].empty() && !nonDAG_condensation.edges[i].empty()) {
            sinks.insert(i);
        }
    }

    nonDAG_condensation.shrink_to_fit();
    reversed_condensation.shrink_to_fit();

    std::set<int> sources_copy(sources);

    if (sources.empty() && sinks.empty()) {
        std::cout << 0 << '\n';
        return 0;
    }
    std::cout << std::max(sources.size(), sinks.size()) + isolated.size() << '\n';


    // searching for source-sink pairs

    vector<int> ordered_sources;
    vector<int> ordered_sinks;

    vector<bool> used(curr + 1, false);

    int w;
    bool not_found;
    size_t p = 0;
    for (auto& el : sources_copy) {

        if (!used[el]) {

            w = 0;
            not_found = true;

            condensation.st(used, w, not_found, el, sinks);

            if (w != 0) {
                ordered_sinks.push_back(w);
                ordered_sources.push_back(el);
                sources.erase(el);
                sinks.erase(w);
                ++p;
            }

        }

    }

    condensation.shrink_to_fit();
    used.shrink_to_fit();


    // algorithm itself

    int last = 1;

    if (!isolated.empty()) {

        std::cout << encrypt[isolated.front()] << ' ';

        for (size_t i = 1; i < isolated.size(); ++i) {
            std::cout << encrypt[isolated[i]] << '\n' << encrypt[isolated[i]] << ' ';
            last = encrypt[isolated[i]];
        }

        std::cout << encrypt[ordered_sources.front()] << '\n';

        for (size_t i = 0; i < p - 1; ++i) {
            std::cout << encrypt[ordered_sinks[i]] << ' ' << encrypt[ordered_sources[i + 1]] << '\n';
            last = encrypt[ordered_sinks[i]];
        }

        std::cout << encrypt[ordered_sinks.back()] << ' ' << encrypt[isolated.front()] << '\n';

    } else {

        for (size_t i = 0; i < p; ++i) {
            std::cout << encrypt[ordered_sinks[i]] << ' ' << encrypt[ordered_sources[(i + 1) % p]] << '\n';
            last = encrypt[ordered_sinks[i]];
        }

    }

    vector<int> sinks_;
    vector<int> sources_;

    sinks_.reserve(sinks.size());
    sources_.reserve(sources.size());

    for (auto& el : sinks) {
        sinks_.push_back(el);
    }

    for (auto& el : sources) {
        sources_.push_back(el);
    }

    for (size_t i = 0; i < std::min(sinks_.size(), sources_.size()); ++i) {
        std::cout << encrypt[sinks_[i]] << ' ' << encrypt[sources_[i]] << '\n';
        last = encrypt[sinks_[i]];
    }

    if (sinks_.size() < sources_.size()) {
        for (size_t i = sinks_.size(); i < sources_.size(); ++i) {
            std::cout << last << ' ' << encrypt[sources_[i]] << '\n';
        }
    } else {
        for (size_t i = sources_.size(); i < sinks_.size(); ++i) {
            std::cout << encrypt[sinks_[i]] << ' ' << last << '\n';
        }
    }

}