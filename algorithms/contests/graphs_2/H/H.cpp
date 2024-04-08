#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <numeric>


using std::vector;
using std::array;
using std::string;
using std::queue;


struct min {
    int operator()(int lhs, int rhs) {
        return std::min(lhs, rhs);
    }
};


class PermutationGraph {

public:

    static constexpr array<int, 11> factorials = {1, 1, 2, 6, 24, 120, 720, 5040, 40'320, 362'880, 3'628'880};

    static int encrypt(vector<int>& v) {
        int res = 0;
        int inv;
        for (size_t i = 0; i < v.size(); ++i) {
            inv = 0;
            for (size_t j = i + 1; j < v.size(); ++j) {
                if (v[i] > v[j]) {
                    ++inv;
                }
            }
            res += inv * factorials[v.size() - i - 1];
        }
        return res + 1;
    }

    static int next_unused(vector<bool>& used, int n) {
        int j;
        int count = 0;
        for (j = 1; j < used.size(); ++j) {
            if (!used[j]) {
                ++count;
            }
            if (n == count) {
                return j;
            }
        }
        return j;
    }

    static void decrypt(int code, vector<int>& v, size_t size) {
        vector<bool> used(size, false);
        v.clear();
        int r;
        int next;
        for (size_t i = 0; i < size; ++i) {
            r = (code - 1) / factorials[size - i - 1] + 1;
            next = next_unused(used, r);
            v.push_back(next);
            used[next] = true;
            code = (code - 1) % factorials[size - i - 1] + 1;
        }
    }

public:

    static const constexpr int inf = std::numeric_limits<int>::max() / 2;

    static int BidirectionalBFS(vector<int>& s, vector<int>& t) {

        size_t size = s.size();

        vector<int> dist_s(factorials[s.size()] + 1, inf);
        vector<int> dist_t(factorials[s.size()] + 1, inf);

        dist_s[encrypt(s)] = 0;
        dist_t[encrypt(t)] = 0;

        vector<bool> used_s(factorials[s.size()] + 1, false);
        vector<bool> used_t(factorials[s.size()] + 1, false);

        queue<int> q_s;
        queue<int> q_t;

        q_s.push(encrypt(s));
        q_t.push(encrypt(t));

        int v;
        int code;
        vector<int> tmp;
        while (!q_s.empty() || !q_t.empty()) {

            v = q_s.front();
            q_s.pop();
            decrypt(v, tmp, s.size());

            used_s[v] = true;
            if (used_t[v]) {
                return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());
            }

            for (size_t i = 0; i < size - 1; ++i) {
                for (size_t j = i + 2; j <= size; ++j) {

                    std::reverse(tmp.begin() + i, tmp.begin() + j);

                    code = encrypt(tmp);

                    if (used_t[code]) {
                        return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());
                    }

                    if (dist_s[code] == inf) {
                        dist_s[code] = dist_s[v] + 1;
                        q_s.push(code);
                    }

                    std::reverse(tmp.begin() + i, tmp.begin() + j);

                }
            }


            v = q_t.front();
            q_t.pop();
            decrypt(v, tmp, t.size());

            used_t[v] = true;
            if (used_s[v]) {
                return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());
            }

            for (size_t i = 0; i < size - 1; ++i) {
                for (size_t j = i + 2; j <= size; ++j) {

                    std::reverse(tmp.begin() + i, tmp.begin() + j);

                    code = encrypt(tmp);

                    if (used_s[code]) {
                        return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());
                    }

                    if (dist_t[code] == inf) {
                        dist_t[code] = dist_t[v] + 1;
                        q_t.push(code);
                    }

                    std::reverse(tmp.begin() + i, tmp.begin() + j);

                }
            }

        }

        return std::transform_reduce(dist_s.begin(), dist_s.end(), dist_t.begin(), inf, min(), std::plus<>());

    }

};


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    size_t n;
    std::cin >> n;

    vector<int> v_s(n);
    for (auto& el : v_s) {
        std::cin >> el;
    }

    vector<int> v_t(n);
    for (auto& el : v_t) {
        std::cin >> el;
    }

    std::cout << PermutationGraph::BidirectionalBFS(v_s, v_t) << '\n';

}