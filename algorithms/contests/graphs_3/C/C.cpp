#include <iostream>
#include <numeric>
#include <vector>
#include <set>
#include <map>


using std::vector;
using std::pair;
using std::set;
using std::map;


size_t min(size_t a, size_t b, size_t c) {
    return std::min(a, std::min(b, c));
}


class Graph {

private:

    static constexpr size_t inf = std::numeric_limits<size_t>::max();

    size_t V;

public:
    vector<size_t> vertexes;

private:
    vector<vector<pair<size_t/*vertex*/, size_t/*weight*/>>> offers_list;

public:

    explicit Graph(size_t V): V(V), vertexes(V + 1), offers_list(V + 1) {}

    ~Graph() = default;

    void make_offer(size_t u, size_t v, size_t weight) {
        offers_list[u].emplace_back(v, weight);
        offers_list[v].emplace_back(u, weight);
    }

    size_t MST_Prim() {

        if (V == 0) {
            return 0;
        }   // :)

        vector<bool> used(V + 1, false);
        vector<size_t> d(V + 1, inf);
        set<pair<size_t/*weight of min edge leading to this vertex*/, size_t/*vertex*/>> unused;
        set<pair<size_t/*weight of vertex*/, size_t/*vertex*/>> unused_;
        set<pair<size_t/*weight of vertex*/, size_t/*vertex*/>> used_;

        for (size_t i = 1; i <= V; ++i) {
            unused_.emplace(vertexes[i], i);
        }

        d[1] = 0;
        unused.emplace(0, 1);

        size_t res = 0;

        while (!unused_.empty()) {

            size_t new_vertex;

            auto [to_by_offer_w, to_by_offer] = unused.empty() ? pair{inf, 0zu} : *unused.begin();

            auto [to_direct_w, to_direct] = used_.empty() ? pair{inf, 0zu} : *unused_.begin();
            auto [from_direct_w, from_direct] = *used_.begin();

            if (used_.empty() || to_by_offer_w < to_direct_w + from_direct_w) {
                unused.erase(unused.begin());
                unused_.erase({vertexes[to_by_offer], to_by_offer});
                used[to_by_offer] = true;
                used_.emplace(vertexes[to_by_offer], to_by_offer);
                new_vertex = to_by_offer;
                res += to_by_offer_w;
            } else {
                unused_.erase(unused_.begin());
                unused.erase({d[to_direct], to_direct});
                used[to_direct] = true;
                used_.emplace(to_direct_w, to_direct);
                new_vertex = to_direct;
                res += to_direct_w + from_direct_w;
            }

            for (auto& [to, w] : offers_list[new_vertex]) {

                if (used[to]) {
                    continue;
                }

                unused.erase({d[to], to});
                d[to] = std::min(d[to], w);
                unused.emplace(d[to], to);

            }

        }

        return res;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V, m;
    std::cin >> V >> m;

    Graph G(V);

    for (size_t i = 0; i < V; ++i) {
        std::cin >> G.vertexes[i + 1];
    }

    size_t u, v, weight;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> u >> v >> weight;
        G.make_offer(u, v, weight);
    }

    std::cout << G.MST_Prim() << '\n';

}