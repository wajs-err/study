#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>


using std::vector;
using std::pair;


class Graph {

private:

    size_t V;
    vector<vector<size_t>> adjacency_matrix;

public:

    explicit Graph(size_t V): V(V), adjacency_matrix(V, vector<size_t>(V, 0)) {}

    ~Graph() = default;

    void add_edge(size_t u, size_t v) {
        adjacency_matrix[u][v] = 1;
        adjacency_matrix[v][u] = 1;
    }

    pair<vector<size_t>, vector<size_t>> min_cut_Stoer_Wagner() {

        auto cut_cost = std::numeric_limits<size_t>::max();
        vector<size_t> half_cut;

        vector<size_t> weight;
        vector<bool> in_set;
        vector<bool> exist(V, true);
        vector<vector<size_t>> vertexes(V, vector<size_t>(1));  // vertexes[i] is vector of vertexes compressed into i
        for (size_t i = 0; i < V; ++i) {
            vertexes[i][0] = i;
        }

        for (size_t i = 0; i < V - 1; ++i) {

            weight.assign(V, 0);
            in_set.assign(V, false);

            size_t curr;
            size_t prev;
            for (size_t j = 0; j < V - i; ++j) {

                curr = V + 1;

                for (size_t k = 0; k < V; ++k) {

                    if (exist[k] && !in_set[k] && (curr > V || weight[k] > weight[curr])) {
                        curr = k;
                    }

                }

                if (j == V - i - 1) {

                    for (size_t k = 0; k < V; ++k) {
                        adjacency_matrix[prev][k] += adjacency_matrix[curr][k];
                        adjacency_matrix[k][prev] = adjacency_matrix[prev][k];
                    }

                    vertexes[prev].insert(vertexes[prev].end(), vertexes[curr].begin(), vertexes[curr].end());
                    exist[curr] = false;

                    if (weight[curr] < cut_cost) {
                        cut_cost = weight[curr];
                        half_cut = vertexes[curr];
                    }

                } else {

                    for (size_t k = 0; k < V; ++k) {
                        weight[k] += adjacency_matrix[curr][k];
                    }

                    in_set[curr] = true;
                    prev = curr;

                }

            }

        }

        pair<vector<size_t>, vector<size_t>> cut;

        cut.first = std::move(half_cut);
        std::sort(cut.first.begin(), cut.first.end());

        size_t j = 0;
        for (size_t i = 0; i < V; ++i) {
            if (j >= cut.first.size() || cut.first[j] > i) {
                cut.second.push_back(i);
            } else {
                ++j;
            }
        }

        return cut;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t V;
    std::cin >> V;

    Graph G(V);

    char c;
    for (size_t i = 0; i < V; ++i) {
        for (size_t j = 0; j < V; ++j) {
            std::cin >> c;
            if (c == '1') {
                G.add_edge(i, j);
            }
        }
    }

    auto cut = G.min_cut_Stoer_Wagner();

    for (auto& v : cut.first) {
        std::cout << v + 1 << ' ';
    }
    std::cout << '\n';

    for (auto& v : cut.second) {
        std::cout << v + 1 << ' ';
    }
    std::cout << '\n';

}