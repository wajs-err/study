// There are probably some bugs there :)
// It is not necessary to have height in CrazyImplicitTreap::Node
// Plus code is pretty shitty, because there a lot of copypaste in it


#include <iostream>
#include <vector>
#include <random>


#pragma GCC target("avx2")
#pragma GCC optimization("O3")
#pragma GCC optimization("unroll-loops")


class CrazyImplicitTreap {

private:

    struct Node {

        Node(int64_t value)
                : left(nullptr), right(nullptr), value(value), priority(random_value()),
                  sum(value), size(1), promise_assign(false), promise_increase(false),
                  promise_val(0), promise_reverse(false), is_increasing(true),
                  is_decreasing(true), most_left(value), most_right(value), height(1) {}

        ~Node() = default;

        Node* left;
        Node* right;
        int64_t value;
        int priority;
        int64_t sum;
        int size;
        bool promise_assign;
        bool promise_increase;
        int64_t promise_val;
        bool promise_reverse;
        bool is_increasing;
        bool is_decreasing;
        int64_t most_left;
        int64_t most_right;
        int height;

    };

public:

    CrazyImplicitTreap() : root(nullptr) {}

    explicit CrazyImplicitTreap(const std::vector<int64_t>& v) : root(nullptr) {

        root = new Node(v[0]);

        std::vector<Node*> prev_nodes;
        prev_nodes.reserve(40);
        prev_nodes.push_back(root);

        Node* node;
        for (size_t i = 1; i < v.size(); ++i) {

            node = new Node(v[i]);

            if (node->priority > prev_nodes.back()->priority) {

                prev_nodes.back()->right = node;
                prev_nodes.push_back(node);

            } else {

                while (!prev_nodes.empty() && node->priority <= prev_nodes.back()->priority) {
                    prev_nodes.pop_back();
                }

                if (prev_nodes.empty()) {
                    node->left = root;
                    node->size += node->left->size;
                    root = node;
                } else {
                    node->left = prev_nodes.back()->right;
                    node->size += node->left->size;
                    prev_nodes.back()->right = node;
                }

                prev_nodes.push_back(node);

            }

        }

        fix(root);

    }

    ~CrazyImplicitTreap() {
        destroy(root);
    }

    // insert element to position pos
    void insert(int pos, int64_t value) {

        Node* v = new Node(value);

        if (root == nullptr) {
            root = v;
            return;
        }

        auto [l, r] = split_by_size(pos, root);
        root = merge(merge(l, v), r);

    }

    // erase element in position pos
    void erase(int pos) {
        auto [l, r] = split_by_size(pos + 1, root);
        auto [l_, x] = split_by_size(pos, l);
        root = merge(l_, r);
        delete x;
    }

    // find element with position pos
    int64_t operator[](int pos) {

        int count = 0;
        auto [l, r] = split_by_size(pos + 1, root);
        count = 0;
        auto [l_, x] = split_by_size(pos, l);

        int64_t res = x->value;

        root = merge(merge(l_, x), r);

        return res;

    }

    // find sum of elements with indexes [l; r]
    int64_t sum(int l, int r) {

        auto [a, b] = split_by_size(r + 1, root);
        auto [c, d] = split_by_size(l, a);

        int64_t res = d->sum;

        root = merge(merge(c, d), b);

        return res;

    }

    // assign value on [l; r]
    void assign(int l, int r, int64_t value) {

        auto [a, b] = split_by_size(r + 1, root);
        auto [c, d] = split_by_size(l, a);

        d->promise_increase = false;
        d->promise_assign = true;
        d->promise_val = value;
        d->value = value;
        d->most_left = value;
        d->most_right = value;
        d->sum = value * d->size;
        d->is_increasing = true;
        d->is_decreasing = true;

        root = merge(merge(c, d), b);

    }

    // increase numbers in [l; r]
    void increase(int l, int r, int64_t value) {

        auto [a, b] = split_by_size(r + 1, root);
        auto [c, d] = split_by_size(l, a);

        d->promise_increase = !d->promise_assign;
        d->promise_val += value;
        d->value += value;
        d->most_left += value;
        d->most_right += value;
        d->sum += value * d->size;

        root = merge(merge(c, d), b);

    }

    // next_permutation of [l; r] (same as std::next_permutation)
    void next_permutation(int l, int r) {

        auto [a, b] = split_by_size(r + 1, root);
        push(a);
        int size = decreasing_suffix_size(a);

        auto [c, d] = split_by_size(std::max(l, a->size - size), a);

        if (size >= r - l + 1) {
            d->promise_reverse ^= true;
            root = merge(merge(c, d), b);
            return;
        }

        auto left_swap_value = c->most_right;
        auto [pos, right_swap_value] = find_by_key(d, left_swap_value, std::less());
        update_single_node(c, c->size - 1, right_swap_value);
        update_single_node(d, pos, left_swap_value);

        d->promise_reverse ^= true;

        root = merge(merge(c, d), b);

    }

    // prev_permutation of [l; r] (same as std::prev_permutation)
    void prev_permutation(int l, int r) {

        auto [a, b] = split_by_size(r + 1, root);
        push(a);
        int size = increasing_suffix_size(a);

        auto [c, d] = split_by_size(std::max(l, a->size - size), a);

        if (size >= r - l + 1) {
            d->promise_reverse ^= true;
            root = merge(merge(c, d), b);
            return;
        }

        auto left_swap_value = c->most_right;
        auto [pos, right_swap_value] = find_by_key(d, left_swap_value, std::greater());
        update_single_node(c, c->size - 1, right_swap_value);
        update_single_node(d, pos, left_swap_value);

        d->promise_reverse ^= true;

        root = merge(merge(c, d), b);

    }

    int size() {
        return root != nullptr ? root->size : 0;
    }

    int height() {
        return root != nullptr ? root->height : 0;
    }

    void print() {
        print(root);
    }

private:

    Node* root;

    static std::pair<Node*, Node*> split_by_size(int size, Node* node) {

        std::pair<Node*, Node*> p = {nullptr, nullptr};

        if (node == nullptr) {
            return p;
        }

        push(node);
        push(node->left);
        push(node->right);

        if ((node->left == nullptr ? 0 : node->left->size) >= size) {

            if (node->left != nullptr) {
                node->sum -= node->left->sum;
                node->size -= node->left->size;
            }

            p = split_by_size(size, node->left);
            push(p.first);
            push(p.second);

            node->left = p.second;
            if (p.second != nullptr) {
                node->sum += p.second->sum;
                node->size += p.second->size;
            }

            node->most_left = p.second == nullptr ? node->value : p.second->most_left;

            node->is_increasing =
                    (node->left == nullptr || node->left->is_increasing) &&
                    (node->right == nullptr || node->right->is_increasing) &&
                    (node->left == nullptr || node->left->most_right <= node->value) &&
                    (node->right == nullptr || node->value <= node->right->most_left);

            node->is_decreasing =
                    (node->left == nullptr || node->left->is_decreasing) &&
                    (node->right == nullptr || node->right->is_decreasing) &&
                    (node->left == nullptr || node->left->most_right >= node->value) &&
                    (node->right == nullptr || node->value >= node->right->most_left);

            node->height = std::max((node->left != nullptr ? node->left->height : 0),
                                    (node->right != nullptr ? node->right->height : 0)) + 1;

            if (p.first != nullptr) {

                p.first->is_increasing =
                        (p.first->left == nullptr || p.first->left->is_increasing) &&
                        (p.first->right == nullptr || p.first->right->is_increasing) &&
                        (p.first->left == nullptr || p.first->left->most_right <= p.first->value) &&
                        (p.first->right == nullptr || p.first->value <= p.first->right->most_left);

                p.first->is_decreasing =
                        (p.first->left == nullptr || p.first->left->is_decreasing) &&
                        (p.first->right == nullptr || p.first->right->is_decreasing) &&
                        (p.first->left == nullptr || p.first->left->most_right >= p.first->value) &&
                        (p.first->right == nullptr || p.first->value >= p.first->right->most_left);

                p.first->height = std::max((p.first->left != nullptr ? p.first->left->height : 0),
                                           (p.first->right != nullptr ? p.first->right->height : 0)) + 1;

            }

            return {p.first, node};

        } else {

            if (node->right != nullptr) {
                node->sum -= node->right->sum;
                node->size -= node->right->size;
            }

            p = split_by_size(size - (node->left == nullptr ? 0 : node->left->size) - 1,
                              node->right);
            push(p.first);
            push(p.second);

            node->right = p.first;
            if (p.first != nullptr) {
                node->sum += p.first->sum;
                node->size += p.first->size;
            }

            node->most_right = p.first == nullptr ? node->value : p.first->most_right;

            node->is_increasing =
                    (node->left == nullptr || node->left->is_increasing) &&
                    (node->right == nullptr || node->right->is_increasing) &&
                    (node->left == nullptr || node->left->most_right <= node->value) &&
                    (node->right == nullptr || node->value <= node->right->most_left);

            node->is_decreasing =
                    (node->left == nullptr || node->left->is_decreasing) &&
                    (node->right == nullptr || node->right->is_decreasing) &&
                    (node->left == nullptr || node->left->most_right >= node->value) &&
                    (node->right == nullptr || node->value >= node->right->most_left);

            node->height = std::max((node->left != nullptr ? node->left->height : 0),
                                    (node->right != nullptr ? node->right->height : 0)) + 1;

            if (p.second != nullptr) {

                p.second->is_increasing =
                        (p.second->left == nullptr || p.second->left->is_increasing) &&
                        (p.second->right == nullptr || p.second->right->is_increasing) &&
                        (p.second->left == nullptr || p.second->left->most_right <= p.second->value) &&
                        (p.second->right == nullptr || p.second->value <= p.second->right->most_left);

                p.second->is_decreasing =
                        (p.second->left == nullptr || p.second->left->is_decreasing) &&
                        (p.second->right == nullptr || p.second->right->is_decreasing) &&
                        (p.second->left == nullptr || p.second->left->most_right >= p.second->value) &&
                        (p.second->right == nullptr || p.second->value >= p.second->right->most_left);

                p.second->height = std::max((p.second->left != nullptr ? p.second->left->height : 0),
                                            (p.second->right != nullptr ? p.second->right->height : 0)) + 1;

            }

            return {node, p.second};

        }

        return p;

    }

    static Node* merge(Node* a, Node* b) {

        push(a);
        push(b);
        if (a != nullptr) {
            push(a->left);
            push(a->right);
        }
        if (b != nullptr) {
            push(b->left);
            push(b->right);
        }

        if (a == nullptr) {
            return b;
        }

        if (b == nullptr) {
            return a;
        }

        if (a->priority < b->priority) {

            if (a->right != nullptr) {
                a->sum -= a->right->sum;
                a->size -= a->right->size;
                a->height = (a->left != nullptr ? a->left->height : 0) + 1;
            }

            a->right = merge(a->right, b);
            a->sum += a->right->sum;
            a->size += a->right->size;

            a->most_right = a->right->most_right;

            a->is_increasing =
                    (a->left == nullptr || a->left->is_increasing) &&
                    (a->right == nullptr || a->right->is_increasing) &&
                    (a->left == nullptr || a->left->most_right <= a->value) &&
                    (a->right == nullptr || a->value <= a->right->most_left);

            a->is_decreasing =
                    (a->left == nullptr || a->left->is_decreasing) &&
                    (a->right == nullptr || a->right->is_decreasing) &&
                    (a->left == nullptr || a->left->most_right >= a->value) &&
                    (a->right == nullptr || a->value >= a->right->most_left);

            a->height = std::max((a->left != nullptr ? a->left->height : 0),
                                 (a->right != nullptr ? a->right->height : 0)) + 1;

            return a;

        } else {

            if (b->left != nullptr) {
                b->sum -= b->left->sum;
                b->size -= b->left->size;
                b->height = (b->right != nullptr ? b->right->height : 0) + 1;
            }

            b->left = merge(a, b->left);
            b->sum += b->left->sum;
            b->size += b->left->size;

            b->most_left = b->left->most_left;

            b->is_increasing =
                    (b->left == nullptr || b->left->is_increasing) &&
                    (b->right == nullptr || b->right->is_increasing) &&
                    (b->left == nullptr || b->left->most_right <= b->value) &&
                    (b->right == nullptr || b->value <= b->right->most_left);

            b->is_decreasing =
                    (b->left == nullptr || b->left->is_decreasing) &&
                    (b->right == nullptr || b->right->is_decreasing) &&
                    (b->left == nullptr || b->left->most_right >= b->value) &&
                    (b->right == nullptr || b->value >= b->right->most_left);

            b->height = std::max((b->left != nullptr ? b->left->height : 0),
                                 (b->right != nullptr ? b->right->height : 0)) + 1;

            return b;

        }

    }

    static void push(Node* node) {

        if (node == nullptr) {
            return;
        }

        if (node->promise_reverse) {
            std::swap(node->left, node->right);
            std::swap(node->most_left, node->most_right);
            std::swap(node->is_increasing, node->is_decreasing);
            if (node->left != nullptr) {
                node->left->promise_reverse ^= true;
            }
            if (node->right != nullptr) {
                node->right->promise_reverse ^= true;
            }
            node->promise_reverse = false;
        }

        if (!node->promise_assign && !node->promise_increase) {
            return;
        }

        if (node->left != nullptr) {
            if (node->promise_assign) {
                node->left->promise_assign = true;
                node->left->promise_increase = false;
                node->left->promise_val = node->promise_val;
                node->left->sum = node->left->size * node->promise_val;
                node->left->value = node->promise_val;
                node->left->most_left = node->promise_val;
                node->left->most_right = node->promise_val;
                node->left->promise_reverse = false;
                node->left->is_increasing = true;
                node->left->is_decreasing = true;
            } else {
                node->left->promise_increase = !node->left->promise_assign;
                node->left->promise_val += node->promise_val;
                node->left->sum += node->left->size * node->promise_val;
                node->left->value += node->promise_val;
                node->left->most_left += node->promise_val;
                node->left->most_right += node->promise_val;
            }
        }

        if (node->right != nullptr) {
            if (node->promise_assign) {
                node->right->promise_assign = true;
                node->right->promise_increase = false;
                node->right->promise_val = node->promise_val;
                node->right->sum = node->right->size * node->promise_val;
                node->right->value = node->promise_val;
                node->right->most_left = node->promise_val;
                node->right->most_right = node->promise_val;
                node->right->promise_reverse = false;
                node->right->is_increasing = true;
                node->right->is_decreasing = true;
            } else {
                node->right->promise_increase = !node->right->promise_assign;
                node->right->promise_val += node->promise_val;
                node->right->sum += node->right->size * node->promise_val;
                node->right->value += node->promise_val;
                node->right->most_left += node->promise_val;
                node->right->most_right += node->promise_val;
            }
        }

        node->promise_assign = false;
        node->promise_increase = false;
        node->promise_val = 0;

    }

    static void destroy(Node* node) {

        if (node == nullptr) {
            return;
        }

        if (node->right != nullptr) {
            destroy(node->right);
        }

        if (node->left != nullptr) {
            destroy(node->left);
        }

        delete node;

    }

    static int random_value() {
        static constexpr auto inf = std::numeric_limits<int>::max();
        static std::random_device device;
        static std::mt19937 random_engine(device());
        static std::uniform_int_distribution<> distribution{-inf, inf};
        return distribution(random_engine);
    }

    // only for sorted segments
    template <typename Comp>
        requires std::is_invocable_v<Comp, int, int>
    static std::pair<int, int64_t> find_by_key(Node* node, int64_t key, const Comp& comp) {

        int pos = 0;
        int64_t value;

        int i = 0;

        while (node != nullptr) {
            ++i;
            if (i > 45) {
                throw "bgfnsdk";
            }
            push(node);
            if (comp(key, node->value) /* key < node->value */ ) {
                pos += (node->left != nullptr ? node->left->size : 0) + 1;
                value = node->value;
                node = node->right;
            } else {
                node = node->left;
            }
        }

        return {pos - 1, value};

    }

    static int increasing_suffix_size(Node* node) {

        if (node == nullptr) {
            return 0;
        }

        if (node->is_increasing) {
            return node->size;
        }

        Node* right = node->right;
        Node* left = node->left;

        push(right);
        int suffix_size = increasing_suffix_size(right);

        if (right == nullptr || right->is_increasing) {

            if (right == nullptr || node->value <= right->most_left) {
                ++suffix_size;
            } else {
                return suffix_size;
            }

            push(left);
            if (left != nullptr && node->value >= left->most_right) {
                suffix_size += increasing_suffix_size(left);
            }

        }

        return suffix_size;

    }

    static int decreasing_suffix_size(Node* node) {

        if (node == nullptr) {
            return 0;
        }

        if (node->is_decreasing) {
            return node->size;
        }

        Node* right = node->right;
        Node* left = node->left;

        push(right);
        int suffix_size = decreasing_suffix_size(right);

        if (right == nullptr || right->is_decreasing) {

            if (right == nullptr || node->value >= right->most_left) {
                ++suffix_size;
            } else {
                return suffix_size;
            }

            push(left);
            if (left != nullptr && node->value <= left->most_right) {
                suffix_size += decreasing_suffix_size(left);
            }

        }

        return suffix_size;

    }

    static void print(Node* node) {

        if (node == nullptr) {
            return;
        }

        push(node);
        print(node->left);
        std::cout << node->value << ' ';
        print(node->right);

    }

    static void fix(Node* node) {

        if (node == nullptr) {
            return;
        }

        fix(node->left);
        fix(node->right);

        node->is_increasing =
                (node->left == nullptr || node->left->is_increasing) &&
                (node->right == nullptr || node->right->is_increasing) &&
                (node->left == nullptr || node->left->most_right <= node->value) &&
                (node->right == nullptr || node->value <= node->right->most_left);

        node->is_decreasing =
                (node->left == nullptr || node->left->is_decreasing) &&
                (node->right == nullptr || node->right->is_decreasing) &&
                (node->left == nullptr || node->left->most_right >= node->value) &&
                (node->right == nullptr || node->value >= node->right->most_left);

        node->sum = (node->left != nullptr ? node->left->sum : 0) +
                (node->right != nullptr ? node->right->sum : 0) + node->value;

        node->size = (node->left != nullptr ? node->left->size : 0) +
                    (node->right != nullptr ? node->right->size : 0) + 1;

        node->most_left = (node->left == nullptr ? node->value : node->left->most_left);
        node->most_right = (node->right == nullptr ? node->value : node->right->most_right);

        node->height = std::max((node->left != nullptr ? node->left->height : 0),
                                (node->right != nullptr ? node->right->height : 0)) + 1;

    }

    static void update_single_node(Node* node, int pos, int64_t value) {

        push(node);

        int curr_pos = (node->left != nullptr ? node->left->size : 0);

        if (pos == curr_pos) {
            push(node->left);
            push(node->right);
            node->value = value;
        }
        if (pos < curr_pos) {
            push(node->right);
            update_single_node(node->left, pos, value);
        }
        if (pos > curr_pos) {
            push(node->left);
            update_single_node(node->right, pos - curr_pos - 1, value);
        }

        node->sum = (node->left != nullptr ? node->left->sum : 0) +
                    (node->right != nullptr ? node->right->sum : 0) + node->value;

        node->most_right = node->right != nullptr ? node->right->most_right : node->value;
        node->most_left = node->left != nullptr ? node->left->most_left : node->value;

        node->is_increasing =
                (node->left == nullptr || node->left->is_increasing) &&
                (node->right == nullptr || node->right->is_increasing) &&
                (node->left == nullptr || node->left->most_right <= node->value) &&
                (node->right == nullptr || node->value <= node->right->most_left);

        node->is_decreasing =
                (node->left == nullptr || node->left->is_decreasing) &&
                (node->right == nullptr || node->right->is_decreasing) &&
                (node->left == nullptr || node->left->most_right >= node->value) &&
                (node->right == nullptr || node->value >= node->right->most_left);

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<int64_t> v(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> v[i];
    }

    CrazyImplicitTreap treap(v);

    int k;
    std::cin >> k;

    int64_t x;
    int query, l, r, pos;
    for (int i = 0; i < k; ++i) {

        std::cin >> query;

        if (query == 1) {
            std::cin >> l >> r;
            std::cout << treap.sum(l, r) << '\n';
            continue;
        }

        if (query == 2) {
            std::cin >> x >> pos;
            treap.insert(pos, x);
            continue;
        }

        if (query == 3) {
            std::cin >> pos;
            treap.erase(pos);
            continue;
        }

        if (query == 4) {
            std::cin >> x >> l >> r;
            treap.assign(l, r, x);
            continue;
        }

        if (query == 5) {
            std::cin >> x >> l >> r;
            treap.increase(l, r, x);
            continue;
        }

        if (query == 6) {
            std::cin >> l >> r;
            treap.next_permutation(l, r);
            continue;
        }

        if (query == 7) {
            std::cin >> l >> r;
            treap.prev_permutation(l, r);
            continue;
        }

    }

    treap.print();
    std::cout << '\n';

}