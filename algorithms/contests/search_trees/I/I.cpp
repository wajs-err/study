#pragma GCC target("avx2")
#pragma GCC optimization("O3")
#pragma GCC optimization("unroll-loops")


#include <iostream>
#include <vector>
#include <random>


template <typename T>
class Treap {

private:

    struct Node {

        Node(const T& value, int priority)
                : left(nullptr), right(nullptr), value(value), priority(priority), size(1) {}

        Node* left;
        Node* right;
        T value;
        int priority;
        int size;

    };

public:

    Treap() : root(nullptr) {}

    ~Treap() {
        destroy(root);
    }

    void insert(const T& value, Node* v) {

        if (v != nullptr) {
            v->value = value;
            v->priority = random_value();
        } else {
            v = new Node(value, random_value());
        }

        if (root == nullptr) {
            root = v;
            return;
        }

        std::vector<Node*> stack;
        stack.reserve(100);
        Node* where = smth(stack, v);

        if (stack.size() == 1) {
            auto [l, r] = split(value, root);
            root = v;
            v->left = l;
            v->right = r;
            v->size += (l != nullptr ? l->size : 0) + (r != nullptr ? r->size : 0);
            return;
        }

        for (int i = 0; i < stack.size() - 1; ++i) {
            ++stack[i]->size;
        }

        if (where == nullptr) {
            if (stack[stack.size() - 2]->value <= value) {
                stack[stack.size() - 2]->right = v;
            } else {
                stack[stack.size() - 2]->left = v;
            }
            return;
        }

        if (stack[stack.size() - 2]->left == where) {
            stack[stack.size() - 2]->left = nullptr;
            auto [l, r] = split(value, where);
            v->left = l;
            v->right = r;
            v->size += (l != nullptr ? l->size : 0) + (r != nullptr ? r->size : 0);
            stack[stack.size() - 2]->left = v;
        } else {
            stack[stack.size() - 2]->right = nullptr;
            auto [l, r] = split(value, where);
            v->left = l;
            v->right = r;
            v->size += (l != nullptr ? l->size : 0) + (r != nullptr ? r->size : 0);
            stack[stack.size() - 2]->right = v;
        }

    }

    Node* erase(const T& value) {

        std::vector<Node*> stack;
        stack.reserve(100);
        Node* del = smth2(stack, value);

        Node* l = del->left;
        Node* r = del->right;
        Node* ins = merge(l, r);

        if (stack.size() != 1) {
            if (stack[stack.size() - 2]->left == del) {
                stack[stack.size() - 2]->left = ins;
            } else {
                stack[stack.size() - 2]->right = ins;
            }
        } else {
            root = ins;
        }

        for (int i = 0; i < stack.size() - 1; ++i) {
            --stack[i]->size;
        }

        del->left = nullptr;
        del->right = nullptr;
        del->size = 1;
        return del;

    }

    void update(const T& old_value, const T& new_value) {
        insert(new_value, erase(old_value));
    }

    Node* get_root() {
        return root;
    }

    Node* smth(std::vector<Node*>& stack, Node* node) {

        if (root == nullptr) {
            return nullptr;
        }

        Node* v = root;

        while (v != nullptr && v->priority < node->priority) {
            if (node->value >= v->value) {
                stack.push_back(v);
                v = v->right;
            } else {
                stack.push_back(v);
                v = v->left;
            }
        }

        stack.push_back(v);
        return stack.back();

    }

    Node* smth2(std::vector<Node*>& stack, const T& value) {

        if (root == nullptr) {
            return nullptr;
        }

        Node* v = root;

        while (v->value != value) {
            if (value > v->value) {
                stack.push_back(v);
                v = v->right;
            } else {
                stack.push_back(v);
                v = v->left;
            }
        }

        stack.push_back(v);
        return stack.back();

    }

    // returns index of max element lesser or equal to the given
    // aka upper_bound or something
    int previous_index(const T& value) {

        Node* v = root;

        int index = (v != nullptr ? root->size : 0);

        while (v != nullptr) {
            if (value < v->value) {
                index -= (v->right != nullptr ? v->right->size : 0) + 1;
                v = v->left;
            } else {
                v = v->right;
            }
        }

        return index;

    }

    // v should be sorted
    // *this should be empty
    // same as Treap(const std::vector<int>&)
    void build(const std::vector<T>& v, int from, int to) {

        root = new Node(v[from], random_value());

        std::vector<Node*> prev_nodes;
        prev_nodes.reserve(100);
        prev_nodes.push_back(root);

        int priority;
        Node* node;
        for (int i = from + 1; i <= to; ++i) {

            priority = random_value();
            node = new Node(v[i], priority);

            if (priority > prev_nodes.back()->priority) {

                prev_nodes.back()->right = node;
                prev_nodes.push_back(node);

            } else {

                while (!prev_nodes.empty() && priority <= prev_nodes.back()->priority) {
                    prev_nodes.back()->size +=
                            (prev_nodes.back()->right != nullptr ? prev_nodes.back()->right->size : 0);
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

        while (!prev_nodes.empty()) {
            prev_nodes.back()->size += (prev_nodes.back()->right != nullptr ? prev_nodes.back()->right->size : 0);
            prev_nodes.pop_back();
        }

    }

private:

    Node* root;

    // first contains all values <= value, second contains all values > value
    std::pair<Node*, Node*> split(const T& value, Node* v) {

        std::pair<Node*, Node*> p = {nullptr, nullptr};

        if (v->value > value) {

            if (v->left != nullptr) {
                p = split(value, v->left);
                v->left = p.second;
                if (p.first != nullptr) {
                    v->size -= p.first->size;
                }
            }

            return {p.first, v};

        }

        if (v->value <= value) {

            if (v->right != nullptr) {
                p = split(value, v->right);
                v->right = p.first;
                if (p.second != nullptr) {
                    v->size -= p.second->size;
                }
            }

            return {v, p.second};

        }

        return p;

    }

    Node* merge(Node* a, Node* b) {

        if (a == nullptr) {
            return b;
        }

        if (b == nullptr) {
            return a;
        }

        if (a->priority < b->priority) {
            a->size -= a->right != nullptr ? a->right->size : 0;
            a->right = merge(a->right, b);
            a->size += a->right->size;
            return a;
        } else {
            b->size -= b->left != nullptr ? b->left->size : 0;
            b->left = merge(a, b->left);
            b->size += b->left->size;
            return b;
        }

    }

    void destroy(Node* v) {

        if (v == nullptr) {
            return;
        }

        if (v->right != nullptr) {
            destroy(v->right);
        }

        if (v->left != nullptr) {
            destroy(v->left);
        }

        delete v;

    }

    static int random_value() {
        static constexpr int inf = 2'147'483'647;
        static std::random_device device;
        static std::mt19937 random_engine(device());
        static std::uniform_int_distribution<> distribution{-inf - 1, inf};
        return distribution(random_engine);
    }

};


class MergeSortTree {

public:

    explicit MergeSortTree(std::vector<int>&& v) : root(nullptr), copy(v), v(std::move(v)) {
        root = build(root, 0, copy.size() - 1);
    }

    ~MergeSortTree() {
        destroy(root);
    }

    // count of numbers n in [l, r] such that x <= n <= y
    int query(int l, int r, int x, int y) {
        return query(root, l, r, x, y);
    }

    void update(int x, int value) {
        update(root, x, value);
    }

private:

    struct Node {

        Node(int l, int r) : l(l), r(r), segment(), left_child(nullptr), right_child(nullptr) {}

        int l;
        int r;
        Treap<int> segment;
        Node* left_child;
        Node* right_child;

    };

    Node* root;
    std::vector<int> copy;
    std::vector<int> v;

    Node* build(Node* node, int from, int to) {

        if (from == to) {
            node = new Node(from, to);
            node->segment.insert(v[from], nullptr);
            return node;
        }

        node = new Node(from, to);

        int m = (from + to) / 2;

        node->left_child = build(node->left_child, from, m);
        node->right_child = build(node->right_child, m + 1, to);

        for (int i = from; i <= m; ++i) {
            copy[i] = v[i];
        }

        int f = from, s = m + 1;
        while (f <= m && s <= to) {
            if (copy[f] < v[s]) {
                v[f + s - m - 1] = copy[f];
                ++f;
            } else {
                v[f + s - m - 1] = v[s];
                ++s;
            }
        }

        for ( ; f <= m; ++f) {
            v[f + s - m - 1] = copy[f];
        }

        for ( ; s <= to; ++s) {
            v[f + s - m - 1] = v[s];
        }

        node->segment.build(v, from, to);

        return node;

    }

    void destroy(Node* node) {

        if (node->left_child != nullptr) {
            destroy(node->left_child);
        }

        if (node->right_child != nullptr) {
            destroy(node->right_child);
        }

        delete node;

    }

    static int query(Node* node, int l, int r, int x, int y) {

        if (node->l == l && node->r == r) {
            return node->segment.previous_index(y) - node->segment.previous_index(x - 1);
        }

        int m = (node->l + node->r) / 2;

        int res = 0;
        if (node->left_child != nullptr && l <= m) {
            res += query(node->left_child, l, std::min(m, r), x, y);
        }
        if (node->right_child != nullptr && r > m) {
            res += query(node->right_child, std::max(m + 1, l), r, x, y);
        }

        return res;

    }

    static int update(Node* node, int x, int value) {

        if (node->r == x && node->l == x) {
            auto root_node = node->segment.get_root();
            int res = root_node->value;
            root_node->value = value;
            return res;
        }

        int prev_value;
        prev_value = update(x <= (node->r + node->l) / 2 ? node->left_child : node->right_child, x, value);

        node->segment.update(prev_value, value);

        return prev_value;

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n, m;
    std::cin >> n >> m;

    std::vector<int> v(n);
    for (auto& el : v) {
        std::cin >> el;
    }

    MergeSortTree mst(std::move(v));

    std::string s = "a";
    int l, r, x, y;
    int coord, val;
    for (int i = 0; i < m; ++i) {
        std::cin >> s;
        if (s[0] == 'G') {
            std::cin >> l >> r >> x >> y;
            --l; --r;
            std::cout << mst.query(l, r, x, y) << '\n';
        } else {
            std::cin >> coord >> val;
            --coord;
            mst.update(coord, val);
        }
    }

}