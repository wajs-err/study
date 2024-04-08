#include <iostream>
#include <algorithm>
#include <vector>


using std::vector;
using std::pair;


// I just understood that all of my previous classes called "DAG" aren't DAGs, they are just directed graphs
class BipartiteDAG {

private:

    size_t L;
    size_t R;
    size_t V;
    vector<vector<size_t>> edges;
    vector<size_t> unmatched_left;

    void dfs(size_t v, vector<bool>& used) {
        used[v] = true;
        for (auto& to : edges[v]) {
            if (!used[to]) {
                dfs(to, used);
            }
        }
    }

public:

    explicit BipartiteDAG(size_t L, size_t R, vector<size_t>&& unmatched_left)
            : L(L), R(R), V(L + R), edges(V + 1), unmatched_left(std::move(unmatched_left)) {}

    ~BipartiteDAG() = default;

    void add_edge(size_t u, size_t v) {
        edges[u].push_back(v);
    }

    // only if *this is correctly directed bipartite graph
    pair<vector<size_t>, vector<size_t>> min_vertex_cover_bipartite() {

        vector<bool> used(V + 1, false);

        for (auto& v : unmatched_left) {
            dfs(v, used);
        }

        pair<vector<size_t>, vector<size_t>> coverage;

        for (size_t i = 1; i <= V; ++i) {

            if (i <= L && !used[i]) {
                coverage.first.push_back(i);
                continue;
            }

            if (i > L && used[i]) {
                coverage.second.push_back(i - L);
            }

        }

        std::sort(coverage.first.begin(), coverage.first.end());
        std::sort(coverage.second.begin(), coverage.second.end());

        return coverage;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t L, R;
    std::cin >> L >> R;

    vector<vector<size_t>> edges(L + 1);

    size_t count;
    for (size_t i = 1; i <= L; ++i) {

        std::cin >> count;
        edges[i].resize(count);
        for (auto& el : edges[i]) {
            std::cin >> el;
        }

    }

    vector<size_t> matching(L + 1);
    vector<size_t> unmatched_left;

    for (size_t i = 1; i <= L; ++i) {

        std::cin >> matching[i];

        if (matching[i] == 0) {
            unmatched_left.push_back(i);
            continue;
        }

        for (auto it = edges[i].begin(); it != edges[i].end(); ++it) {
            if (*it == matching[i]) {
                edges[i].erase(it);
                break;
            }
        }

    }

    BipartiteDAG G(L, R, std::move(unmatched_left));

    for (size_t i = 1; i <= L; ++i) {
        if (matching[i] != 0) {
            G.add_edge(L + matching[i], i);
        }
    }

    for (size_t i = 1; i <= L; ++i) {
        for (auto& v : edges[i])  {
            G.add_edge(i, L + v);
        }
    }

    auto [l, r] = G.min_vertex_cover_bipartite();

    std::cout << l.size() + r.size() << '\n';

    std::cout << l.size() << ' ';
    for (auto& v : l) {
        std::cout << v << ' ';
    }
    std::cout << '\n';

    std::cout << r.size() << ' ';
    for (auto& v : r) {
        std::cout << v << ' ';
    }
    std::cout << '\n';

}