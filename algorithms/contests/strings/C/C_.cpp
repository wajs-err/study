#include <iostream>
#include <vector>
#include <queue>
#include <array>
#include <map>


class Trie {

private:

    struct Node {

        Node() : children_(), numbers_(0), is_terminal_(false) {}

        Node(bool is_terminal, int number) : Node() {
            is_terminal_ = is_terminal;
            numbers_.push_back(number);
        }

        std::map<int, int> children_;
        std::vector<int> numbers_;
        bool is_terminal_;

    };

    std::vector<Node> nodes_;
    size_t word_count_;

    void build_suffix_links(std::vector<int>& suffix_link) {

        std::vector<std::array<int, 26>> go(nodes_.size());

        suffix_link[0] = 0;
        for (int c = 0; c < 26; ++c) {
            go[0][c] = nodes_[0].children_.contains(c) ? nodes_[0].children_[c] : 0;
        }

        std::queue<int> q;
        q.push(0);

        int v, u;
        while (!q.empty()) {
            v = q.front();
            q.pop();
            for (int c = 0; c < 26; ++c) {
                if (!nodes_[v].children_.contains(c)) {
                    continue;
                }
                u = nodes_[v].children_[c];
                q.push(u);
                suffix_link[u] = v != 0 ? go[suffix_link[v]][c] : 0;
                if (v != 0) {
                    for (const auto& el : nodes_[go[suffix_link[v]][c]].numbers_) {
                        nodes_[u].numbers_.push_back(el);
                    }
                }
                for (int d = 0; d < 26; ++d) {
                    go[u][d] = nodes_[u].children_.contains(d)
                               ? nodes_[u].children_[d]
                               : go[suffix_link[u]][d];
                }
            }
        }

    }

public:

    Trie() : nodes_(1), word_count_(0) {}

    void insert(const std::string& s, int number) {
        int node_index = 0;
        for (const auto& c : s) {
            if (!nodes_[node_index].children_.contains(c - 'a')) {
                nodes_[node_index].children_[c - 'a'] = nodes_.size();
                nodes_.emplace_back();
            }
            node_index = nodes_[node_index].children_[c - 'a'];
        }
        ++word_count_;
        nodes_[node_index].is_terminal_ = true;
        nodes_[node_index].numbers_.push_back(number);
    }

    bool contains(const std::string& s) {
        int node_index = 0;
        for (const auto& c : s) {
            if (!nodes_[node_index].children_.contains(c - 'a')) {
                return false;
            }
            node_index = nodes_[node_index].children_[c - 'a'];
        }
        return nodes_[node_index].is_terminal_;
    }

    std::vector<std::vector<int>> AhoCorasick(const std::string& t) {

        std::vector<int> suffix_link(nodes_.size());
        build_suffix_links(suffix_link);

        std::vector<std::vector<int>> res(word_count_);

        int v = 0;
        for (int i = 0; i < t.size(); ++i) {
            while (!nodes_[v].children_.contains(t[i] - 'a') && v != 0) {
                v = suffix_link[v];
            }
            v = nodes_[v].children_[t[i] - 'a'];
            for (const auto& el : nodes_[v].numbers_) {
                res[el].push_back(i);
            }
        }

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

    for (int i = 0; const auto& positions : trie.AhoCorasick(t)) {
        std::cout << positions.size() << ' ';
        for (const auto& el : positions) {
            std::cout << el - sizes[i] + 2 << ' ';
        }
        std::cout << '\n';
        ++i;
    }

}