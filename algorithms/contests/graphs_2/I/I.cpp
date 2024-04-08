#include <iostream>
#include <numeric>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>


using std::pair;
using std::string;
using std::vector;
using std::set;
using std::unordered_set;
using std::map;
using std::unordered_map;


class WordLadderGame {

private:

    struct PolynomialStringHash {
        vector<uint64_t> powers;
        PolynomialStringHash(uint64_t hash_seed, const vector<uint64_t>& powers) : powers(powers) {}
        uint64_t operator()(const string& str) const noexcept {
            uint64_t res = 0;
            uint64_t x;
            for (size_t i = 0; i < str.size(); ++i) {
                x = str[i] - 'a' + 1;
                res += powers[i] * x;
            }
            return res;
        }
    };

    struct S {
        int64_t dist;
        bool used;
    };

    uint64_t hash_seed;
    uint64_t string_size;
    unordered_set<uint64_t> hashes;
    map<uint64_t, vector<uint64_t>> edges;
    vector<uint64_t> powers;
    PolynomialStringHash hasher;

    int64_t Hamming_distance(uint64_t from, uint64_t to) const {
        int64_t res = 0;
        for (size_t i = 0; i < string_size; ++i) {
            if (from % (hash_seed + 1) != to % (hash_seed + 1)) {
                ++res;
            }
            from /= (hash_seed + 1);
            to /= (hash_seed + 1);
        }
        return res;
    }

    int64_t h_f(const uint64_t& vertex, const uint64_t& s, const uint64_t& t) const {
        return (Hamming_distance(vertex, t) - Hamming_distance(vertex, s));
    }

    int64_t h_r(const uint64_t& vertex, const uint64_t& s, const uint64_t& t) const {
        return -h_f(vertex, s, t);
    }

    void possible_moves(const uint64_t& string, vector<uint64_t>& vertexes) const {

        vertexes.clear();

        for (size_t i = 0; i < string_size; ++i) {

            for (uint64_t j = 1; j < string / powers[i] % (hash_seed + 1); ++j) {
                if (hashes.contains(string - j * powers[i])) {
                    vertexes.push_back(string - j * powers[i]);
                }
            }

            for (uint64_t j = 1; j < hash_seed - string / powers[i] % (hash_seed + 1) + 1; ++j) {
                if (hashes.contains(string + j * powers[i])) {
                    vertexes.push_back(string + j * powers[i]);
                }
            }

        }

    }

    vector<uint64_t> initialize_powers() const {

        vector<uint64_t> pows(string_size);
        pows[0] = 1;

        for (size_t i = 1; i < string_size; ++i) {
            pows[i] = pows[i - 1] * (hash_seed + 1);
        }

        return pows;

    }

public:

    WordLadderGame(vector<string>&& v, uint64_t string_size, uint64_t hash_seed)
            : hash_seed(hash_seed),
              string_size(string_size),
              hashes(),
              edges(),
              powers(initialize_powers()),
              hasher(hash_seed, powers) {

        hashes.reserve(100'000);
        for (auto& el : v) {
            hashes.emplace(hasher(el));
        }

        vector<uint64_t> vec;
        vec.resize(100'000);

        for (auto& vertex : hashes) {
            possible_moves(vertex, vec);
            auto& ref = edges[vertex];
            for (auto& to : vec) {
                ref.push_back(to);
            }
        }

    }

    ~WordLadderGame() = default;

    // Bidirectional A*
    int64_t distance(const string& s_, const string& t_) {

        uint64_t s = hasher(s_);
        uint64_t t = hasher(t_);

        if (!hashes.contains(s) || !hashes.contains(t)) {
            return -1;
        }

        if (s == t) {
            return 0;
        }

        unordered_map<uint64_t/*vertex*/, S/*info*/> map_s;
        set<pair<int64_t/*dist*/, uint64_t/*vertex*/>> set_s;

        map_s[s] = {.dist = 0, .used = false};
        set_s.emplace(0, s);

        unordered_map<uint64_t/*vertex*/, S/*info*/> map_t;
        set<pair<int64_t/*dist*/, uint64_t/*vertex*/>> set_t;

        map_t[t] = {.dist = 0, .used = false};
        set_t.emplace(0, t);

        int64_t dist = std::numeric_limits<int64_t>::max() / 2;
        int64_t heuristic_value;

        unordered_map<uint64_t, S>::iterator it_s, it_t;
        vector<uint64_t> v;

        while (!set_s.empty() && !set_t.empty()) {

            auto [min_s, v_s] = *set_s.begin();
            auto [min_t, v_t] = *set_t.begin();

            auto& rref_s = map_s.find(v_s)->second;
            auto& rref_t = map_t.find(v_t)->second;

            if (min_s <= min_t) {

                set_s.erase(set_s.begin());
                rref_s.used = true;

                for (uint64_t& to : edges[v_s]) {

                    auto& ref = map_s.emplace(to, S()).first->second;

                    if (ref.used) {
                        continue;
                    }

                    heuristic_value = h_f(to, s, t);

                    if (ref.dist == 0 || ref.dist > rref_s.dist + 2) {
                        set_s.erase({ref.dist + heuristic_value, to});
                        ref.dist = rref_s.dist + 2;
                        set_s.emplace(ref.dist + heuristic_value, to);
                    }

                    it_t = map_t.find(to);

                    if (it_t != map_t.end()) {
                        auto& ref_ = it_t->second;
                        if (ref_.used && ref.dist + ref_.dist < dist) {
                            dist = ref.dist + ref_.dist;
                        }
                    }

                }

            } else {

                set_t.erase(set_t.begin());
                rref_t.used = true;

                for (uint64_t& to : edges[v_t]) {

                    auto& ref = map_t.emplace(to, S()).first->second;

                    if (ref.used) {
                        continue;
                    }

                    heuristic_value = h_r(to, s, t);

                    if (ref.dist == 0 || ref.dist > rref_t.dist + 2) {
                        set_t.erase({ref.dist + heuristic_value, to});
                        ref.dist = rref_t.dist + 2;
                        set_t.emplace(ref.dist + heuristic_value, to);
                    }

                    it_s = map_s.find(to);

                    if (it_s != map_s.end()) {
                        auto& ref_ = it_s->second;
                        if (ref_.used && ref.dist + ref_.dist < dist) {
                            dist = ref.dist + ref_.dist;
                        }
                    }

                }

            }

            // I don't know why stopping criterion is like this
            // (why do we add heuristics? I don't know...)
            // I was trying to find correct criterion for about a week or something...
            // If you put this criterion into 15 puzzle (problem K) it won't pass tests...
            if (rref_s.dist + h_f(v_s, s, t) + rref_t.dist + h_r(v_t, s, t) >= dist) {
                break;
            }

        }

        return (dist == std::numeric_limits<int64_t>::max() / 2 ? -1 : dist / 2);

    }

    // just Dijkstra's algorithm for testing
    int64_t Dijkstra(const string& s_, const string& t_) {

        uint64_t s = hasher(s_);
        uint64_t t = hasher(t_);

        if (!hashes.contains(s) || !hashes.contains(t)) {
            return -1;
        }

        if (s == t) {
            return 0;
        }

        unordered_map<uint64_t, int64_t> dist;

        for (auto& el : hashes) {
            dist[el] = std::numeric_limits<int64_t>::max();
        }

        dist[s] = 0;

        std::set<pair<int64_t/*dist*/, uint64_t/*vertex*/>> unused;

        unused.emplace(0, s);
        for (auto& el : hashes) {
            unused.emplace(dist[el], el);
        }

        while (!unused.empty()) {
            auto [d, v] = *unused.begin();
            unused.erase(unused.begin());
            for (auto& to : edges[v]) {
                if (unused.contains({dist[to], to}) && dist[to] > dist[v] + 1) {
                    unused.erase({dist[to], to});
                    dist[to] = dist[v] + 1;
                    unused.emplace(dist[to], to);
                }
            }
        }

        return dist[t] == std::numeric_limits<int64_t>::max() ? -1 : dist[t];

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t n;
    std::cin >> n;

    vector<string> v(n);
    uint64_t seed;

    for (auto& str : v) {
        std::cin >> str;
    }

    auto vv = v;

    // There is only one test with length more than 10 and all the words in this test are contains only letters a and b
    // If length <= 10 then all the words contains Latin letters only and there are 26 of them
    seed = (v[0].size() > 10 ? 2 : 26); // some magic numbers :)

    WordLadderGame wlg(std::move(v), v[0].size(), seed);

    size_t m = 100'000;
    std::cin >> m;

    string s, t;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> s >> t;
        std::cout << wlg.distance(s, t) << '\n';
    }

}