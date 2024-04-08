#include <iostream>


template <typename T>
class SplayTree {

private:

    struct Node {
        Node(): parent(nullptr), left(nullptr), right(nullptr), value(T()), subtree_height(0) {}
        Node* parent;
        Node* left;
        Node* right;
        T value;
        int subtree_height;
    };

public:

    SplayTree(): static_root(nullptr) {}

    ~SplayTree() {
        destroy(static_root);
    }

    void insert(T value) {

        if (static_root == nullptr) {
            static_root = new Node;
            Node* root = new Node;
            static_root->value = value;
            root->value = value;
            static_root->left = root;
            root->parent = static_root;
            return;
        }

        if (static_root->left == nullptr) {
            Node* root = new Node;
            static_root->value = value;
            root->value = value;
            static_root->left = root;
            root->parent = static_root;
            return;
        }

        Node* v = static_root->left;
        Node* new_node = new Node;
        new_node->value = value;
        new_node->subtree_height = 1;

        while (v != nullptr) {

            if (value < v->value) {
                if (v->left != nullptr) {
                    v = v->left;
                } else {
                    v->left = new_node;
                    new_node->parent = v;
                    splay(new_node);
                    return;
                }
            }

            if (value > v->value) {
                if (v->right != nullptr) {
                    v = v->right;
                } else {
                    v->right = new_node;
                    new_node->parent = v;
                    splay(new_node);
                    return;
                }
            }

            if (value == v->value) {
                return;
            }

        }

    }

    void erase(T value) {

        if (static_root == nullptr || static_root->left == nullptr) {
            return;
        }

        Node* v = static_root->left;
        bool is_left = true;

        while (v->value != value) {

            if (value < v->value) {
                v = v->left;
                is_left = true;
            } else {
                v = v->right;
                is_left = false;
            }

            if (v == nullptr) {
                return;
            }

        }

        if (v->left == nullptr && v->right == nullptr) {
            (is_left ? v->parent->left : v->parent->right) = nullptr;
            Node* tmp = v->parent;
            delete v;

            if (tmp != static_root) {
                splay(tmp);
            } else {
                tmp->left = nullptr;
            }

            return;
        }

        if (v->left == nullptr) {
            (is_left ? v->parent->left : v->parent->right) = v->right;
            v->right->parent = v->parent;
            Node* tmp = v->right;
            delete v;
            splay(tmp);
            return;
        }

        if (v->right == nullptr) {
            (is_left ? v->parent->left : v->parent->right) = v->left;
            v->left->parent = v->parent;
            Node* tmp = v->left;
            delete v;
            splay(tmp);
            return;
        }

        Node* new_v = v->right;

        bool f = false;
        while (new_v->left != nullptr) {
            new_v = new_v->left;
            f = true;
        }

        if (new_v->right != nullptr) {
            new_v->right->parent = new_v->parent;
        }

        v->value = new_v->value;
        (f ? new_v->parent->left : new_v->parent->right) = new_v->right;
        Node* tmp = (new_v->right == nullptr ? new_v->parent : new_v->right);
        delete new_v;
        splay(tmp);

    }

    bool contains(T value) {

        if (static_root == nullptr || static_root->left == nullptr) {
            return false;
        }

        Node* v = static_root->left;

        while (v->value != value) {

            if (value < v->value) {
                if (v->left == nullptr) {
                    splay(v);
                    return false;
                }
                v = v->left;
            } else {
                if (v->right == nullptr) {
                    splay(v);
                    return false;
                }
                v = v->right;
            }

        }

        return true;

    }

    // returns min element greater than the given
    T next(T value) {

        if (static_root == nullptr || static_root->left == nullptr) {
            return value;
        }

        Node* v = static_root->left;
        T res = value;

        while (v != nullptr) {
            if (value < v->value) {
                res = v->value;
                if (v->left == nullptr) {
                    splay(v);
                    return res;
                }
                v = v->left;
            } else {
                if (v->right == nullptr) {
                    splay(v);
                    return res;
                }
                v = v->right;
            }
        }

        return res;

    }

    // returns max element lesser than the given
    T previous(T value) {

        if (static_root == nullptr || static_root->left == nullptr) {
            return value;
        }

        Node* v = static_root->left;
        T res = value;

        while (v != nullptr) {
            if (value > v->value) {
                res = v->value;
                if (v->right == nullptr) {
                    splay(v);
                    return res;
                }
                v = v->right;
            } else {
                if (v->left == nullptr) {
                    splay(v);
                    return res;
                }
                v = v->left;
            }
        }

        return res;

    }

private:

    Node* static_root;

    void splay(Node* v) {

        while (v->parent != static_root) {

            if (v->parent->parent != static_root) {

                Node* grandparent = v->parent->parent;
                Node* parent = v->parent;

                if (grandparent->left == parent && parent->left == v) {
                    zigzig(grandparent);
                }

                if (grandparent->left == parent && parent->right == v) {
                    zigzag(grandparent);
                }

                if (grandparent->right == parent && parent->left == v) {
                    zagzig(grandparent);
                }

                if (grandparent->right == parent && parent->right == v) {
                    zagzag(grandparent);
                }

            } else {

                if (v->parent->left == v) {
                    zig(v->parent);
                } else {
                    zag(v->parent);
                }

            }

        }

    }

    void zig(Node* v) {

        Node* u = v;

        (v->parent->left == v ? v->parent->left : v->parent->right) = v->left;
        v->left->parent = v->parent;
        v = v->left;
        u->left = v->right;
        if (v->right != nullptr) {
            v->right->parent = u;
        }
        v->right = u;
        u->parent = v;

        u->subtree_height = std::max(u->left == nullptr ? 0 : u->left->subtree_height,
                                     u->right == nullptr ? 0 : u->right->subtree_height) + 1;
        v->subtree_height = std::max(v->left == nullptr ? 0 : v->left->subtree_height,
                                     u->subtree_height) + 1;

    }

    void zag(Node* v) {

        Node* u = v;

        (v->parent->left == v ? v->parent->left : v->parent->right) = v->right;
        v->right->parent = v->parent;
        v = v->right;
        u->right = v->left;
        if (v->left != nullptr) {
            v->left->parent = u;
        }
        v->left = u;
        u->parent = v;

        u->subtree_height = std::max(u->left == nullptr ? 0 : u->left->subtree_height,
                                     u->right == nullptr ? 0 : u->right->subtree_height) + 1;
        v->subtree_height = std::max(v->right == nullptr ? 0 : v->right->subtree_height,
                                     u->subtree_height) + 1;

    }

    void zigzig(Node* v) {
        zig(v);
        zig(v->parent);
    }

    void zigzag(Node* v) {
        zag(v->left);
        zig(v);
    }

    void zagzig(Node* v) {
        zig(v->right);
        zag(v);
    }

    void zagzag(Node* v) {
        zag(v);
        zag(v->parent);
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

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    SplayTree<int> tree;

    std::string s;
    int n;

    while (std::cin >> s) {

        std::cin >> n;

        switch (s[0]) {

            case 'i':
                tree.insert(n);
                break;

            case 'd':
                tree.erase(n);
                break;

            case 'e':
                std::cout << (tree.contains(n) ? "true\n" : "false\n");
                break;

            case 'n':
                std::cout << (n == tree.next(n) ? "none" : std::to_string(tree.next(n))) << '\n';
                break;

            case 'p':
                std::cout << (n == tree.previous(n) ? "none" : std::to_string(tree.previous(n))) << '\n';
                break;

            default:
                break;

        }

    }

}