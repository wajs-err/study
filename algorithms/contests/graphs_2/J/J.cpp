// A*

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>


using std::vector;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::set;
using std::pair;


class Puzzle15 {

public:

    uint64_t permutation;

    static uint64_t convert(vector<uint64_t>&& vector) {

        uint64_t ret = 0;

        for (auto& el : vector) {
            ret <<= 4;
            ret += el;
        }

        return ret;

    }

    static void convert(uint64_t permutation, vector<uint64_t>& vector) {
        size_t j = 15;
        while (permutation != 0) {
            vector[j--] = permutation % 16;
            permutation >>= 4;
        }
    }

    // Manhattan distance from the open position to its correct place
    static uint64_t heuristic(uint64_t from, uint64_t to) {

        uint64_t ret = 0;

        uint64_t p;
        for (uint64_t i = to % 16, pos = 15; to != 0; to >>= 4, --pos, i = to % 16) {

            p = from;
            uint64_t j = 15;

            while (p % 16 != i) {
                --j;
                p >>= 4;
            }

            ret += static_cast<uint64_t>(
                    std::abs(static_cast<int64_t>(pos % 4) - static_cast<int64_t>(j % 4)) +
                    std::abs(static_cast<int64_t>(pos / 4) - static_cast<int64_t>(j / 4)));

        }

        return ret * 5;

    }

    static vector<pair<uint64_t, char>> possible_moves(uint64_t permutation) {

        uint64_t p = permutation;
        uint64_t j = 15;
        while (p % 16 != 0) {
            --j;
            p >>= 4;
        }

        vector<pair<uint64_t, char>> ret;

        if ((j & 3) != 3) {
//            vector<uint64_t> v(16, 0);
//            convert(permutation, v);
//            std::swap(v[j], v[j + 1]);
//            ret.emplace_back(convert(std::move(v)), 'L');
            ret.emplace_back((permutation & ~(15ull << ((15ull - (j + 1ull)) << 2ull))) |
                             ((permutation & (15ull << ((15ull - (j + 1ull)) << 2ull))) << 4ull), 'L');
        }

        if ((j & 3) != 0) {
//            vector<uint64_t> v(16, 0);
//            convert(permutation, v);
//            std::swap(v[j], v[j - 1]);
//            ret.emplace_back(convert(std::move(v)), 'R');
            ret.emplace_back((permutation & ~(15ull << ((15ull - (j - 1ull)) << 2ull))) |
                             ((permutation & (15ull << ((15ull - (j - 1ull)) << 2ull))) >> 4ull), 'R');
        }

        if (j < 12) {
//            vector<uint64_t> v(16, 0);
//            convert(permutation, v);
//            std::swap(v[j], v[(j + 4)]);
//            ret.emplace_back(convert(std::move(v)), 'U');
            ret.emplace_back((permutation & ~(15ull << ((15ull - (j + 4ull)) << 2ull))) |
                             ((permutation & (15ull << ((15ull - (j + 4ull)) << 2ull))) << 16ull), 'U');
        }

        if (j > 3) {
//            vector<uint64_t> v(16, 0);
//            convert(permutation, v);
//            std::swap(v[j], v[(j - 4)]);
//            ret.emplace_back(convert(std::move(v)), 'D');
            ret.emplace_back((permutation & ~(15ull << ((15ull - (j - 4ull)) << 2ull))) |
                             ((permutation & (15ull << ((15ull - (j - 4ull)) << 2ull))) >> 16ull), 'D');
        }

        return ret;

    }

    // like Dijkstra but with heuristic
    std::string Bidirectional_A_star(uint64_t s, uint64_t t) {

        unordered_map<uint64_t/*vertex*/, uint64_t/*dist*/> dist_s;
        unordered_map<uint64_t/*vertex*/, uint64_t/*dist*/> heuristic_dist_s;
        unordered_set<uint64_t/*vertex*/> used_s;
        unordered_map<uint64_t/*vertex*/, pair<char/*move*/, uint64_t/*from*/>> parent_s;
        set<pair<uint64_t/*dist*/, uint64_t/*vertex*/>> current_s;

        parent_s.emplace(std::piecewise_construct,
                         std::forward_as_tuple(s), std::forward_as_tuple('\0', 0));
        dist_s[s] = 0;
        heuristic_dist_s[s] = heuristic(s, t);
        current_s.emplace(heuristic(s, t), s);

        unordered_map<uint64_t/*vertex*/, uint64_t/*dist*/> dist_t;
        unordered_map<uint64_t/*vertex*/, uint64_t/*dist*/> heuristic_dist_t;
        unordered_set<uint64_t/*vertex*/> used_t;
        unordered_map<uint64_t/*vertex*/, pair<char/*move*/, uint64_t/*from*/>> parent_t;
        set<pair<uint64_t/*dist*/, uint64_t/*vertex*/>> current_t;

        parent_t.emplace(std::piecewise_construct,
                         std::forward_as_tuple(t), std::forward_as_tuple('\0', 0));
        dist_t[t] = 0;
        heuristic_dist_t[t] = heuristic(t, s);
        current_t.emplace(heuristic(t, s), t);

        uint64_t dist;
        uint64_t last_vertex;

        while (!current_s.empty() || !current_t.empty()) {

            auto [min_s, v_s] = *current_s.begin();
            auto [min_t, v_t] = *current_t.begin();

            if (v_s == v_t) {
                dist = dist_s[v_t] + dist_t[v_t];
                last_vertex = v_t;
                break;
            }

            if (min_s <= min_t) {

                if (dist_t.contains(v_s)) {
                    dist = dist_s[v_s] + dist_t[v_s];
                    last_vertex = v_s;
                    break;
                }

                current_s.erase(current_s.begin());
                used_s.emplace(v_s);

                for (auto [to, move] : possible_moves(v_s)) {

                    if (used_s.contains(to)) {
                        continue;
                    }

                    if (!dist_s.contains(to) || dist_s[to] > dist_s[v_s] + 1) {
                        current_s.erase({heuristic_dist_s[to], to});
                        dist_s[to] = dist_s[v_s] + 1;
                        heuristic_dist_s[to] = dist_s[to] + heuristic(to, t);
                        current_s.emplace(heuristic_dist_s[to], to);
                        parent_s.emplace(std::piecewise_construct,
                                         std::forward_as_tuple(to), std::forward_as_tuple(move, v_s));
                    }

                }

            } else {

                if (dist_s.contains(v_t)) {
                    dist = dist_s[v_t] + dist_t[v_t];
                    last_vertex = v_t;
                    break;
                }

                current_t.erase(current_t.begin());
                used_t.emplace(v_t);

                for (auto [to, move] : possible_moves(v_t)) {

                    if (used_t.contains(to)) {
                        continue;
                    }

                    if (!dist_t.contains(to) || dist_t[to] > dist_t[v_t] + 1) {
                        current_t.erase({heuristic_dist_t[to], to});
                        dist_t[to] = dist_t[v_t] + 1;
                        heuristic_dist_t[to] = dist_t[to] + heuristic(to, s);
                        current_t.emplace(heuristic_dist_t[to], to);
                        parent_t.emplace(std::piecewise_construct,
                                         std::forward_as_tuple(to), std::forward_as_tuple(move, v_t));
                    }

                }

            }

        }

        std::string str;

        for (uint64_t i = last_vertex; i != s; i = parent_s[i].second) {
            str.push_back(parent_s[i].first);
        }

        std::reverse(str.begin(), str.end());

        for (uint64_t i = last_vertex; i != t; i = parent_t[i].second) {
            if (parent_t[i].first == 'L') {
                str.push_back('R');
            } else if (parent_t[i].first == 'R') {
                str.push_back('L');
            } else if (parent_t[i].first == 'U') {
                str.push_back('D');
            } else if (parent_t[i].first == 'D') {
                str.push_back('U');
            }
        }

//        if (dist != str.size()) {
//            std::cout << dist << '\n' << str.size() << '\n';
//        }

        return str;

    }

public:

    explicit Puzzle15(std::vector<uint64_t>&& vector) : permutation(convert(std::move(vector))) {}

    ~Puzzle15() = default;

    // checks if solution exists
    bool solution_exists() {

        vector<uint64_t> v(16);
        convert(permutation, v);

        int inv = 0;
        for (size_t i = 0; i < v.size(); ++i) {
            for (size_t j = i + 1; j < v.size(); ++j) {
                if (v[j] != 0 && v[i] > v[j]) {
                    ++inv;
                }
            }
        }

        uint64_t p = permutation;
        int j = 15;
        while (p % 16 != 0) {
            --j;
            p >>= 4;
        }

        return (inv + (j / 4) + 1) % 2 == 0;

    }

    // returns number of moves needed (-1 if it's impossible) and string of tiles' moves to solve
    pair<int64_t, string> solve() {

        if (!solution_exists()) {
            return {-1, {}};
        }

        auto s = Bidirectional_A_star(permutation, 0x123456789abcdef0ull);

        return {s.size(), s};

    }

};


int main() {

    vector<uint64_t> p(16);

    for (auto& el : p) {
        std::cin >> el;
    }

    Puzzle15 puzzle(std::move(p));

    auto ans = puzzle.solve();

    std::cout << ans.first << '\n';

    if (ans.first == -1) {
        return 0;
    }

    std::cout << ans.second << '\n';

}

// 1 2 0 3 4 5 6 7 9 8 10 11 12 13 14 15
// 42