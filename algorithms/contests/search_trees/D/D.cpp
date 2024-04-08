#include <iostream>


// uses C-style rand() and srand()
template <typename T = long long>
class TreapImplicit {

private:

    struct Node {
        Node(): left(nullptr), right(nullptr), value(T()), priority(0), sum(T()), subtree_size(0) {}
        Node* left;
        Node* right;
        T value;
        int priority;
        T sum;
        size_t subtree_size;
    };

public:

    TreapImplicit(): root(nullptr) {
        srand(8764531);
    }

    ~TreapImplicit() {
        destroy(root);
    }

    // insert by position
    void insert_by_pos(size_t pos, T value) {

        Node* v = new Node;
        v->value = value;
        v->sum = value * value;
        v->subtree_size = 1;
        v->priority = rand() % random_range;

        if (root == nullptr) {
            root = v;
            return;
        }

        if (pos > root->subtree_size) {
            pos = root->subtree_size;
        }

        auto [l, r] = split_by_size(pos, root);
        root = merge(merge(l, v), r);

    }

    // erase by position
    void erase_by_pos(size_t pos) {

        if (pos > root->subtree_size) {
            return;
        }

        auto [l, r] = split_by_size(pos + 1, root);
        auto [l_, x] = split_by_size(pos, l);

        delete x;

        root = merge(l_, r);

    }

    bool contains(T value) {

        if (root == nullptr) {
            return false;
        }

        Node* v = root;

        while (v->value != value) {

            if (value < v->value) {
                if (v->left == nullptr) {
                    return false;
                }
                v = v->left;
            } else {
                if (v->right == nullptr) {
                    return false;
                }
                v = v->right;
            }

        }

        return true;

    }

    T find_by_pos(size_t pos) {

        if (root == nullptr || pos > root->subtree_size) {
            return T();
        }

        auto [l, r] = split_by_size(pos + 1, root);
        auto [l_, x] = split_by_size(pos, l);

        T res = x->value;

        root = merge(merge(l_, x), r);

        return res;

    }

    // returns min element greater than the given
    T next(T value) {

        if (root == nullptr) {
            return value;
        }

        Node* v = root;
        T res = value;

        while (v != nullptr) {
            if (value < v->value) {
                res = v->value;
                v = v->left;
            } else {
                v = v->right;
            }
        }

        return res;

    }

    // returns max element lesser than the given
    T previous(T value) {

        if (root == nullptr) {
            return value;
        }

        Node* v = root;
        T res = value;

        while (v != nullptr) {
            if (value > v->value) {
                res = v->value;
                v = v->right;
            } else {
                v = v->left;
            }
        }

        return res;

    }

    // find sum of elements with indexes in [l; r]
    T sum_by_pos(size_t l, size_t r) {

        if (l > root->subtree_size) {
            return {};
        }

        if (r > root->subtree_size) {
            r = root->subtree_size;
        }

        auto [a, b] = split_by_size(l, root);
        if (b == nullptr) {
            return {};
        }

        auto [c, d] = split_by_size(r, b);
        if (c == nullptr) {
            return {};
        }

        T res = l + c->sum;

        Node* tmp = merge(c, d);
        root = merge(a, tmp);

        return res;

    }

    size_t size() {
        return root == nullptr ? 0 : root->subtree_size;
    }

    // may be useful while debugging
    void print() {
        print(root, 0);
        std::cout << "---------------------\n";
    }

private:
public:
    Node* root;
    static const size_t random_range = 1'000'000'000;

    static std::pair<Node*, Node*> split_by_size(size_t size, Node* v) {

        std::pair<Node*, Node*> p = {nullptr, nullptr};

        if (v == nullptr) {
            return p;
        }

        if ((v->left == nullptr ? 0 : v->left->subtree_size) >= size) {

            if (v->left != nullptr) {
                v->sum -= v->left->sum;
                v->subtree_size -= v->left->subtree_size;
            }

            p = split_by_size(size, v->left);

            v->left = p.second;
            if (p.second != nullptr) {
                v->sum += p.second->sum;
                v->subtree_size += p.second->subtree_size;
            }

            return {p.first, v};

        } else {

            if (v->right != nullptr) {
                v->sum -= v->right->sum;
                v->subtree_size -= v->right->subtree_size;
            }

            p = split_by_size(size - (v->left == nullptr ? 0 : v->left->subtree_size) - 1,
                              v->right);

            v->right = p.first;
            if (p.first != nullptr) {
                v->sum += p.first->sum;
                v->subtree_size += p.first->subtree_size;
            }

            return {v, p.second};

        }

    }

    static Node* merge(Node* a, Node* b) {

        if (a == nullptr) {
            return b;
        }

        if (b == nullptr) {
            return a;
        }

        if (a->priority < b->priority) {

            if (a->right != nullptr) {
                a->sum -= a->right->sum;
                a->subtree_size -= a->right->subtree_size;
            }

            Node* u = merge(a->right, b);

            a->right = u;
            if (u != nullptr) {
                a->sum += u->sum;
                a->subtree_size += u->subtree_size;
            }

            return a;

        } else {

            if (b->left != nullptr) {
                b->sum -= b->left->sum;
                b->subtree_size -= b->left->subtree_size;
            }

            Node* u = merge(a, b->left);

            b->left = u;
            if (u != nullptr) {
                b->sum += u->sum;
                b->subtree_size += u->subtree_size;
            }

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

    // may be useful when debugging
    void print(Node* v, size_t depth) {

        if (v == nullptr) {
            return;
        }

        if (v->right != nullptr) {
            print(v->right, depth + 1);
        }

        for (size_t i = 0; i < depth; ++i) {
            std::cout << "    ";
        }

        std::cout << v->value << '\n';

        if (v->left != nullptr) {
            print(v->left, depth + 1);
        }

    }

};
#include <fstream>

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::ifstream in("river.in");
    std::ofstream out("river.out");

    TreapImplicit<long long> t;

    size_t n, p;
    in >> n >> p;

    long long a;
    for (size_t i = 0; i < n; ++i) {
        in >> a;
        t.insert_by_pos(i, a);
    }

    size_t k;
    in >> k;

    int e;
    size_t v;
    long long b;
    for (size_t i = 0; i < k; ++i) {

        out << t.root->sum << '\n';

        in >> e >> v;
        --v;

        if (e == 1) {

            a = t.find_by_pos(v);
            t.erase_by_pos(v);

            if (v == t.size()) {
                b = t.find_by_pos(v - 1);
                t.erase_by_pos(v - 1);
                t.insert_by_pos(v - 1, a + b);
            } else if (v == 0) {
                b = t.find_by_pos(v);
                t.erase_by_pos(v);
                t.insert_by_pos(v, a + b);
            } else {
                b = t.find_by_pos(v - 1);
                t.erase_by_pos(v - 1);
                t.insert_by_pos(v - 1, a / 2 + b);
                b = t.find_by_pos(v);
                t.erase_by_pos(v);
                t.insert_by_pos(v, a - a / 2 + b);
            }

        } else {

            a = t.find_by_pos(v);
            t.erase_by_pos(v);
            t.insert_by_pos(v, a - a / 2);
            t.insert_by_pos(v, a / 2);

        }

    }

    out << t.root->sum << '\n';

}