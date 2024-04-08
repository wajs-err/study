// A*

#include <iostream>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <set>


//#pragma GCC target ("avx2")
//#pragma GCC optimization ("O3")
//#pragma GCC optimization ("unroll-loops")


using std::vector;
using std::string;
using std::unordered_map;
using std::set;
using std::pair;


class Puzzle15 {

private:

    struct S {
        int16_t dist = 0;
        uint64_t parent = 0;
        bool used = false;
    };

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

    // Manhattan distance
    static int16_t heuristic(const uint64_t& from, const uint64_t& to) {

        int16_t ret = 0;
        uint64_t to_ = to;
        uint64_t p;
        uint64_t i;
        int16_t pos = 16;
        int16_t j;

        for (size_t k = 0; k < 16; ++k) {

            i = to_ % 16;
            to_ >>= 4;
            --pos;

            if (i == 0) {
                continue;
            }

            p = from;
            j = 15;

            while (p % 16 != i) {
                --j;
                p >>= 4;
            }

            ret += std::abs(pos % 4 - j % 4) + std::abs(pos / 4 - j / 4);

        }

        return ret;

    }

    static int16_t h_f(const uint64_t& vertex, const uint64_t& s, const uint64_t& t) {
        return (heuristic(vertex, t) - heuristic(vertex, s));
    }

    static int16_t h_r(const uint64_t& vertex, const uint64_t& s, const uint64_t& t) {
        return -h_f(vertex, s, t);
    }

    static void possible_moves(const uint64_t& permutation, vector<uint64_t>& ans) {

        uint64_t p = permutation;
        uint64_t j = 15;
        while (p % 16 != 0) {
            --j;
            p >>= 4;
        }

        // thanks https://github.com/asarandi/n-puzzle for bit magic

        ans.clear();

        if ((j & 3) != 3) {
            ans.push_back((permutation & ~(15ull << ((15ull - (j + 1ull)) << 2ull))) |
                          ((permutation & (15ull << ((15ull - (j + 1ull)) << 2ull))) << 4ull));
        }

        if ((j & 3) != 0) {
            ans.push_back((permutation & ~(15ull << ((15ull - (j - 1ull)) << 2ull))) |
                         ((permutation & (15ull << ((15ull - (j - 1ull)) << 2ull))) >> 4ull));
        }

        if (j < 12) {
            ans.push_back((permutation & ~(15ull << ((15ull - (j + 4ull)) << 2ull))) |
                          ((permutation & (15ull << ((15ull - (j + 4ull)) << 2ull))) << 16ull));
        }

        if (j > 3) {
            ans.push_back((permutation & ~(15ull << ((15ull - (j - 4ull)) << 2ull))) |
                          ((permutation & (15ull << ((15ull - (j - 4ull)) << 2ull))) >> 16ull));
        }

    }

    static char move(uint64_t from, uint64_t to) {

        uint64_t j = 15;
        while (from % 16 != 0) {
            --j;
            from >>= 4;
        }

        uint64_t i = 15;
        while (to % 16 != 0) {
            --i;
            to >>= 4;
        }

        if (i - j == 1) {
            return 'L';
        } else if (j - i == 1) {
            return 'R';
        } else if (i - j == 4) {
            return 'U';
        } else {
            return 'D';
        }

    }

    // like Dijkstra but with heuristic
    static void Bidirectional_A_star(uint64_t s, uint64_t t, string& str) {

        unordered_map<uint64_t/*vertex*/, S/*info*/> from_s;
        set<pair<int16_t/*dist*/, uint64_t/*vertex*/>> current_s;
        from_s.reserve(4'000'000);

        from_s[s] = {.dist = 0, .parent = 0, .used = false};
        current_s.emplace(0, s);

        unordered_map<uint64_t/*vertex*/, S/*info*/> from_t;
        set<pair<int16_t/*dist*/, uint64_t/*vertex*/>> current_t;
        from_t.reserve(4'000'000);

        from_t[t] = {.dist = 0, .parent = 0, .used = false};
        current_t.emplace(0, t);

        int16_t dist = std::numeric_limits<int16_t>::max() / 4;
        uint64_t meet_point = 0;
        int16_t heuristic_value;

        unordered_map<uint64_t, S>::iterator it_s, it_t;
        vector<uint64_t> v(4);

        while (/*!current_s.empty() && !current_t.empty()*/ true) {

            auto [min_s, v_s] = *current_s.begin();
            auto [min_t, v_t] = *current_t.begin();

            auto& rref_s = from_s.find(v_s)->second;
            auto& rref_t = from_t.find(v_t)->second;

            if (rref_s.dist + rref_t.dist >= dist) {
                break;
            }

            if (min_s <= min_t) {

                current_s.erase(current_s.begin());
                rref_s.used = true;

                possible_moves(v_s, v);

                for (uint64_t& to : v) {

                    auto& ref = from_s.emplace(to, S()).first->second;

                    if (ref.used) {
                        continue;
                    }

                    heuristic_value = h_f(to, s, t);

                    if (ref.dist == 0 || ref.dist > rref_s.dist + 2) {
                        current_s.erase({ref.dist + heuristic_value, to});
                        ref.dist = rref_s.dist + 2;
                        current_s.emplace(ref.dist + heuristic_value, to);
                        ref.parent = v_s;
                    }

                    it_t = from_t.find(to);

                    if (it_t != from_t.end()) {
                        auto& ref_ = it_t->second;
                        if (ref_.used && ref.dist + ref_.dist < dist) {
                            dist = ref.dist + ref_.dist;
                            meet_point = to;
                        }
                    }

                }

            } else {

                current_t.erase(current_t.begin());
                rref_t.used = true;

                possible_moves(v_t, v);

                for (uint64_t& to : v) {

                    auto& ref = from_t.emplace(to, S()).first->second;

                    if (ref.used) {
                        continue;
                    }

                    heuristic_value = h_r(to, s, t);

                    if (ref.dist == 0 || ref.dist > rref_t.dist + 2) {
                        current_t.erase({ref.dist + heuristic_value, to});
                        ref.dist = rref_t.dist + 2;
                        current_t.emplace(ref.dist + heuristic_value, to);
                        ref.parent = v_t;
                    }

                    it_s = from_s.find(to);

                    if (it_s != from_s.end()) {
                        auto& ref_ = it_s->second;
                        if (ref_.used && ref.dist + ref_.dist < dist) {
                            dist = ref.dist + ref_.dist;
                            meet_point = to;
                        }
                    }

                }

            }

        }

        for (uint64_t i = meet_point, p = from_s[i].parent; i != s; i = p, p = from_s[p].parent) {
            str.push_back(move(p, i));
        }

        std::reverse(str.begin(), str.end());

        for (uint64_t i = meet_point, p = from_t[i].parent; i != t; i = p, p = from_t[p].parent) {
            str.push_back(move(i, p));
        }

    }

public:

    explicit Puzzle15(std::vector<uint64_t>&& vector) : permutation(convert(std::move(vector))) {}

    ~Puzzle15() = default;

    // checks if solution exists
    bool solution_exists() const {

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
    pair<size_t, string> solve() const {

        if (!solution_exists()) {
            return {-1, {}};
        }

        string s;

        Bidirectional_A_star(permutation, 0x123456789abcdef0ull, s);

        return {s.size(), s};

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

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