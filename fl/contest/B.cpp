#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <array>


class Trie {

private:

    static inline const auto inf = std::numeric_limits<int>::max();

    struct Node {

        Node() : children_(), pattern_numbers_(), height_(0),
                 occurrences_count_(0), position_(inf), is_terminal_(false) {
            for (auto& el : children_) {
                el = 0;
            }
        }

        explicit Node(int height) : Node() {
            height_ = height;
        }

        std::array<int, 26> children_;
        std::vector<int> pattern_numbers_;
        int height_;
        int occurrences_count_;
        int position_;
        bool is_terminal_;

    };

    std::vector<Node> nodes_;
    size_t word_count_;
    int height_;

    void build_suffix_links(std::vector<int>& suffix_link) {

        std::vector<std::array<int, 26>> go(nodes_.size());

        suffix_link[0] = 0;
        for (int c = 0; c < 26; ++c) {
            go[0][c] = nodes_[0].children_[c] != 0 ? nodes_[0].children_[c] : 0;
        }

        std::queue<int> q;
        q.push(0);

        int v, u;
        while (!q.empty()) {
            v = q.front();
            q.pop();
            for (int c = 0; c < 26; ++c) {
                u = nodes_[v].children_[c];
                if (u == 0) {
                    continue;
                }
                q.push(u);
                suffix_link[u] = v != 0 ? go[suffix_link[v]][c] : 0;
                for (int d = 0; d < 26; ++d) {
                    go[u][d] = nodes_[u].children_[d] != 0
                               ? nodes_[u].children_[d]
                               : go[suffix_link[u]][d];
                }
            }
        }

    }

    void accumulate(const std::vector<int>& suffix_link) {

        std::vector<std::vector<int>> layers(height_ + 1);

        for (int i = 0; i < nodes_.size(); ++i) {
            layers[nodes_[i].height_].push_back(i);
        }

        for (int i = height_; i >= 0; --i) {
            for (const auto& v : layers[i]) {
                nodes_[suffix_link[v]].occurrences_count_ += nodes_[v].occurrences_count_;
                nodes_[suffix_link[v]].position_ = std::min(nodes_[v].position_, nodes_[suffix_link[v]].position_);
            }
        }

    }

    void pattern_search(std::vector<std::pair<int, int>>& res) {

        std::queue<int> q;
        q.push(0);

        int v;
        while (!q.empty()) {
            v = q.front();
            q.pop();
            if (nodes_[v].is_terminal_) {
                for (const auto& el : nodes_[v].pattern_numbers_) {
                    res[el].first = nodes_[v].occurrences_count_;
                    res[el].second = nodes_[v].position_;
                }
            }
            for (const auto& child : nodes_[v].children_) {
                if (child != 0) {
                    q.push(child);
                }
            }
        }

    }

public:

    Trie() : nodes_(1), word_count_(0), height_(0) {}

    void insert(const std::string& s, int number) {
        if (s.size() >= height_) {
            height_ = s.size();
        }
        int node_index = 0;
        for (int h = 1; const auto& c : s) {
            if (nodes_[node_index].children_[c - 'a'] == 0) {
                nodes_[node_index].children_[c - 'a'] = nodes_.size();
                nodes_.emplace_back(h);
            }
            node_index = nodes_[node_index].children_[c - 'a'];
            ++h;
        }
        ++word_count_;
        nodes_[node_index].is_terminal_ = true;
        nodes_[node_index].pattern_numbers_.push_back(number);
    }

    bool contains(const std::string& s) {
        int node_index = 0;
        for (const auto& c : s) {
            if (nodes_[node_index].children_[c - 'a'] == 0) {
                return false;
            }
            node_index = nodes_[node_index].children_[c - 'a'];
        }
        return nodes_[node_index].is_terminal_;
    }

    std::vector<std::pair<int, int>> AhoCorasick(const std::string& t) {

        std::vector<int> suffix_link;
        suffix_link.resize(nodes_.size());
        build_suffix_links(suffix_link);

        int v = 0;
        for (int i = 0; i < t.size(); ++i) {
            while (nodes_[v].children_[t[i] - 'a'] == 0 && v != 0) {
                v = suffix_link[v];
            }
            v = nodes_[v].children_[t[i] - 'a'];
            ++nodes_[v].occurrences_count_;
            if (nodes_[v].position_ == inf) {
                nodes_[v].position_ = i + 1;
            }
        }

        accumulate(suffix_link);

        std::vector<std::pair<int/*count*/, int/*index*/>> res(word_count_, {0, 0});
        pattern_search(res);

        return res;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string t;
    std::cin >> t;

    int n;
    std::cin >> n;

    Trie trie;
    std::string s;
    std::vector<int> sizes(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> s;
        sizes[i] = s.size();
        trie.insert(s, i);
    }

    for (size_t i = 0; const auto& [count, pos] : trie.AhoCorasick(t)) {
        std::cout << count << ' ';
        std::cout << (count == 0 ? 0 : pos - sizes[i] + 1) << '\n';
        ++i;
    }

}
