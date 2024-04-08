#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <numeric>


using std::vector;


//#define float long double


class DAG {

    public:

        struct Rate {

            int currency;
            float rate;
            float commission;

            Rate(int currency, float rate, float commission): currency(currency), rate(rate), commission(commission) {}

        };

        size_t V;
        vector<vector<Rate>> g;
        vector<float> amount;

        static bool is_inf(float f) {
            return std::abs(f - minus_inf) <= std::abs(minus_inf) / 100;
        }

        bool FordBellman(int s, float sum) {

            vector<float> prev(V, minus_inf);
            vector<float> curr(V, minus_inf);
            prev[s] = sum;
            curr[s] = sum;

            for (size_t k = 1; k < V; ++k) {

                for (size_t i = 0; i < V; ++i) {

                    float cost = minus_inf;
                    for (size_t j = 0; j < g[i].size(); ++j) {
                        if (!is_inf(prev[g[i][j].currency])
                                && (prev[g[i][j].currency] - g[i][j].commission) * g[i][j].rate > cost) {
                            cost = (prev[g[i][j].currency] - g[i][j].commission) * g[i][j].rate;
                        }
                    }

                    curr[i] = std::max(prev[i], cost);

                }

                prev = curr;

            }

            vector curr_(curr);
            vector prev_(prev);

            for (size_t k = 0; k < V; ++k) {

                for (size_t i = 0; i < V; ++i) {

                    float cost = minus_inf;
                    for (size_t j = 0; j < g[i].size(); ++j) {
                        if (!is_inf(prev_[g[i][j].currency])
                                && (prev_[g[i][j].currency] - g[i][j].commission) * g[i][j].rate > cost) {
                            cost = (prev_[g[i][j].currency] - g[i][j].commission) * g[i][j].rate;
                        }
                    }

                    curr_[i] = std::max(prev_[i], cost);

                }

                prev_ = curr_;

            }

            for (size_t i = 0; i < V; ++i) {
                if (curr_[i] > curr[i] && curr[i] > 0) {
                    return true;
                }
            }

            return false;

        }

    public:

        static constexpr float inf = std::numeric_limits<float>::max();
        static constexpr float minus_inf = std::numeric_limits<float>::min();

        DAG() = delete;
        DAG(size_t V): V(V), g(vector<vector<Rate>>(V)), amount(vector(V, inf)) {}
        ~DAG() = default;

        void push_edge(int u, int v, float rate, float commission) {
            g[v].emplace_back(u, rate, commission); // to v from u
        }

        bool find_positive_cycle(int currency, float sum) {
            return FordBellman(currency, sum);
        }

};


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    size_t n, m;
    std::cin >> n >> m;

    int currency;
    float sum;
    std::cin >> currency >> sum;

    DAG g(n);

    int u, v;
    float rate, commission;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> u >> v >> rate >> commission;
        g.push_edge(u - 1, v - 1, rate, commission);
        std::cin >> rate >> commission;
        g.push_edge(v - 1, u - 1, rate, commission);
    }

    std::cout << (g.find_positive_cycle(currency - 1, sum) ? "YES" : "NO") << '\n';

}
