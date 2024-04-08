#include <iostream>


template <typename T>
class AVL_tree {

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

    AVL_tree(): static_root(nullptr) {}

    ~AVL_tree() {
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
                    balance(v);
                    return;
                }
            }

            if (value > v->value) {
                if (v->right != nullptr) {
                    v = v->right;
                } else {
                    v->right = new_node;
                    new_node->parent = v;
                    balance(v);
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
            balance(tmp);
            return;
        }

        if (v->left == nullptr) {
            (is_left ? v->parent->left : v->parent->right) = v->right;
            v->right->parent = v->parent;
            Node* tmp = v->parent;
            delete v;
            balance(tmp);
            return;
        }

        if (v->right == nullptr) {
            (is_left ? v->parent->left : v->parent->right) = v->left;
            v->left->parent = v->parent;
            Node* tmp = v->parent;
            delete v;
            balance(tmp);
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
        Node* tmp = new_v->parent;
        delete new_v;
        balance(tmp);

    }

    bool contains(T value) {

        if (static_root == nullptr || static_root->left == nullptr) {
            return false;
        }

        Node* v = static_root->left;

        while (v->value != value) {

            if (value < v->value) {
                v = v->left;
            } else {
                v = v->right;
            }

            if (v == nullptr) {
                return false;
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
                v = v->left;
            } else {
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
                v = v->right;
            } else {
                v = v->left;
            }
        }

        return res;

    }

private:

    Node* static_root;

    void balance(Node* v) {

        Node* u = v;

        while (u != static_root) {
            u->subtree_height = std::max(u->left == nullptr ? 0 : u->left->subtree_height,
                                         u->right == nullptr ? 0 : u->right->subtree_height) + 1;
            u = u->parent;
        }

        u = v;
        int h_delta;

        while (u != static_root) {

            h_delta = (u->left == nullptr ? 0 : u->left->subtree_height) -
                      (u->right == nullptr ? 0 : u->right->subtree_height);

            if (h_delta == -2) {

                if ((u->right->left == nullptr ? 0 : u->right->left->subtree_height) -
                    (u->right->right == nullptr ? 0 : u->right->right->subtree_height) <= 0) {
                    rotateLeft(u);
                } else {
                    rotateRightLeft(u);
                }

            }

            if (h_delta == 2) {

                if ((u->left->left == nullptr ? 0 : u->left->left->subtree_height) -
                    (u->left->right == nullptr ? 0 : u->left->right->subtree_height) >= 0) {
                    rotateRight(u);
                } else {
                    rotateLeftRight(u);
                }

            }

            u = u->parent;

        }

    }

    void rotateLeft(Node* v) {

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

    void rotateRight(Node* v) {

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

    void rotateLeftRight(Node* v) {
        rotateLeft(v->left);
        rotateRight(v);
    }

    void rotateRightLeft(Node* v) {
        rotateRight(v->right);
        rotateLeft(v);
    }

    void destroy(Node* v) {
        if (v == nullptr) {
            return;
        }
        destroy(v->right);
        destroy(v->left);
        delete v;
    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    AVL_tree<int> tree;

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