#include <iostream>
#include <limits>
#include <vector>
#include <map>


class SuffixTree {

private:

    struct Node {

        Node(int left, int length) : left_(left), length_(length), suffix_link_(), children_() {}

        Node(int left, int length, int suffix_link)
                : left_(left), length_(length), suffix_link_(suffix_link), children_() {}

        int left_;
        int length_;
        size_t suffix_link_;
        std::map<char, size_t> children_;

    };

    struct State {

        size_t node = 0;
        char edge = '\0';
        int offset = 0;

        bool is_root() const {
            return node == 0 && offset == 0;
        }

    };

    std::string string_;
    std::vector<Node> nodes_;
    static inline const auto inf_ = std::numeric_limits<int>::max();

    void add_char(size_t i, State& current_state) {

        size_t child = 0;
        while (!has_transition(current_state, string_[i]) && !current_state.is_root()) {

            if (current_state.offset != 0) {
                if (child != 0) {
                    nodes_[child].suffix_link_ = nodes_.size();
                }
                child = next_node(current_state);
                next_node(current_state) = nodes_.size();
                nodes_.emplace_back(nodes_[child].left_, current_state.offset);
                nodes_.back().children_[string_[nodes_[child].left_ + current_state.offset]] = child;
                nodes_[child].left_ = nodes_.back().left_ + nodes_.back().length_;
                if (nodes_[child].length_ != inf_) {
                    nodes_[child].length_ -= current_state.offset;
                }
                child = nodes_.size() - 1;
                nodes_.back().children_[string_[i]] = nodes_.size();
                nodes_.emplace_back(i, inf_);
            } else {
                if (child != 0) {
                    nodes_[child].suffix_link_ = current_state.node;
                }
                child = nodes_.size();
                nodes_[current_state.node].children_[string_[i]] = nodes_.size();
                nodes_.emplace_back(i, inf_);
            }

            go_by_suffix_link(current_state);

        }

        if (has_transition(current_state, string_[i])) {
            if (child != 0) {
                nodes_[child].suffix_link_ = current_state.node;
            }
            if (current_state.offset == 0) {
                current_state.edge = string_[i];
            }
            ++current_state.offset;
            if (!nodes_[current_state.node].children_.contains(current_state.edge)) {
                return;
            }
            int from = nodes_[next_node(current_state)].left_;
            reduce_edges(current_state, from);
        } else {
            nodes_[current_state.node].children_[string_[i]] = nodes_.size();
            nodes_.emplace_back(i, inf_);
        }

    }

    void go_by_suffix_link(State& current_state) {

        int from = nodes_[nodes_[current_state.node].children_[current_state.edge]].left_;
        if (current_state.node == 0) {
            --current_state.offset;
            current_state.edge = string_[nodes_[nodes_[current_state.node].children_[current_state.edge]].left_ + 1];
            ++from;
        } else {
            current_state.node = nodes_[current_state.node].suffix_link_; // suffix_link[root] = root?
        }

        reduce_edges(current_state, from);

    }

    void reduce_edges(State& current_state, int from) {

        if (!nodes_[current_state.node].children_.contains(current_state.edge)) {
            return;
        }

        size_t child = nodes_[current_state.node].children_[current_state.edge];
        while (current_state.offset >= nodes_[child].length_) {
            current_state.offset -= nodes_[child].length_;
            current_state.edge = string_[from + nodes_[child].length_];
            from += nodes_[child].length_;
            current_state.node = child;
            if (!nodes_[current_state.node].children_.contains(current_state.edge)) {
                return;
            }
            child = nodes_[current_state.node].children_[current_state.edge];
        }

    }

    bool has_transition(const State& state, char c) {
        return state.offset == 0 && nodes_[state.node].children_.contains(c) ||
            state.offset != 0 && string_[nodes_[nodes_[state.node].children_[state.edge]].left_ + state.offset] == c;
    }

    size_t& next_node(const State& state) {
        return nodes_[state.node].children_[state.edge];
    }

    void print_impl(std::ostream& out, size_t index, int& parent) {
        int p = parent;
        for (const auto& [c, i] : nodes_[index].children_) {
            out << p << ' ' << nodes_[i].left_ << ' ' << (nodes_[i].length_ == inf_
                                                                ? string_.size()
                                                                : nodes_[i].left_ + nodes_[i].length_) << '\n';
            print_impl(out, i, ++parent);
        }
    }

    void print_suffixes_impl(std::ostream& out, size_t index, std::string& s) {

        std::string cp;
        for (const auto& [c, i] : nodes_[index].children_) {
            cp = s;
            for (auto j = nodes_[i].left_;
                    j < (nodes_[i].length_ == inf_ ? string_.size() : nodes_[i].left_ + nodes_[i].length_);
                    ++j) {
                cp += string_[j];
            }
            print_suffixes_impl(out, i, cp);
        }

        if (nodes_[index].children_.empty()) {
            out << s << '\n';
        }

    }

public:

    // Ukkonen's algorithm
    explicit SuffixTree(const std::string& s) : string_(s), nodes_(1, Node{0, 0, 0}) {
        State current_state;
        for (size_t i = 0; i < s.size(); ++i) {
            add_char(i, current_state);
        }
    }

    void print(std::ostream& out) {
        out << nodes_.size() << '\n';
        int parent = 0;
        print_impl(out, 0, parent);
    }

    void print_suffixes(std::ostream& out) {
        std::string s;
        print_suffixes_impl(out, 0, s);
    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string s;
    std::cin >> s;

    SuffixTree suffix_tree(s);

    suffix_tree.print(std::cout);

}