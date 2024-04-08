// Literally shit. This is code is disgusting.
// It's working, and I understand it well, but you shouldn't check it because it's dangerous for mental health.


#include <iostream>
#include <vector>


class BinomialHeap {

public:

    struct Node {
        int value;
        size_t number;
        size_t index;
        Node* parent;
        std::vector<Node*> children;
        explicit Node(int value, size_t index) : value(value), number(), index(index), parent(nullptr), children() {}
    };

    static inline std::vector<BinomialHeap::Node*> nodes; // wtf it's inline?

    std::vector<Node*> trees;
    size_t size_;
    size_t number;

    static Node* merge_binomial_trees(Node*& a, Node*& b) {

        Node* ret;

        if (a->value == b->value && a->index < b->index || a->value < b->value) {
            ret = a;
            ret->children.push_back(b);
            b->parent = ret;
        } else {
            ret = b;
            ret->children.push_back(a);
            a->parent = ret;
        }

        a = nullptr;
        b = nullptr;

        return ret;

    }

    auto get_min_iterator() {
        return std::min_element(trees.begin(), trees.end(), [](Node*& lhs, Node*& rhs){
            if (lhs == nullptr) {
                return false;
            }
            return lhs != nullptr && rhs == nullptr ||
                   lhs->value == rhs->value && lhs->index < rhs->index ||
                   lhs->value < rhs->value;
        });
    }

    void destroy_tree(Node* node) {
        for (auto& el : node->children) {
            destroy_tree(el);
        }
        delete node;
    }

    static Node* sift_up(Node* node) {
        while (node->parent != nullptr) {
            if (node->value == node->parent->value && node->index < node->parent->index ||
                node->value < node->parent->value) {
                std::swap(node->value, node->parent->value);
                std::swap(node->index, node->parent->index);
                std::swap(nodes[node->index], nodes[node->parent->index]);
                node = node->parent;
            } else {
                return node;
            }
        }
        return node;
    }

    BinomialHeap() : trees(), size_(0), number() {}

public:

    explicit BinomialHeap(size_t number) : trees(), size_(0), number(number) {}
    ~BinomialHeap() {
        for (auto& tree : trees) {
            if (tree != nullptr) {
                destroy_tree(tree);
            }
        }
    }

    void merge(BinomialHeap& other) {

        size_ += other.size();
        other.size_ = 0;

        for (auto& el : other.trees) {
            if (el != nullptr) {
                el->number = number;
            }
        }

        bool carry = false;
        Node* carry_ptr = nullptr;

        for (auto i = 0; i < std::max(trees.size(), other.trees.size()) || carry; ++i) {

            if (i == trees.size()) {
                trees.push_back(nullptr);
            }

            if (trees[i] != nullptr && i < other.trees.size() && other.trees[i] != nullptr) {
                carry = true;
                Node* tmp = trees[i];
                trees[i] = carry_ptr;
                tmp = merge_binomial_trees(tmp, other.trees[i]);
                carry_ptr = tmp;
                continue;
            }

            if (trees[i] != nullptr) {
                if (carry) {
                    carry_ptr = merge_binomial_trees(trees[i], carry_ptr);
                }
                continue;
            }

            if (i < other.trees.size() && other.trees[i] != nullptr) {
                if (carry) {
                    carry_ptr = merge_binomial_trees(other.trees[i], carry_ptr);
                } else {
                    trees[i] = other.trees[i];
                    other.trees[i] = nullptr;
                }
                continue;
            }

            trees[i] = carry_ptr;
            carry_ptr = nullptr;
            carry = false;

        }

    }

    Node* insert(const int& value, size_t index) {
        BinomialHeap h;
        auto node = new Node(value, index);
        h.trees.push_back(node);
        ++h.size_;
        merge(h);
        return node;
    }

    int get_min() {
        return (*get_min_iterator())->value;
    }

    void extract_min() {
        --size_;
        BinomialHeap h; // size of this heap is 0 and size of *this is corrected manually
        auto it = get_min_iterator();
        auto index = it - trees.begin();
        for (auto i = 0; i < index; ++i) {
            h.trees.push_back((*it)->children[i]);
            h.trees.back()->parent = nullptr;
        }
        delete *it;
        *it = nullptr;
        merge(h);
    }

    Node* assign(Node* node, const int& value) {

        if (value == node->value) {
            return node;
        }

        if (value < node->value) {
            node->value = value;
            return sift_up(node);
        }

        auto index = node->index;
        erase(node);
        nodes[index] = insert(value, index);
        return nodes[index];

    }

    void erase(Node* node) {
        assign(node, get_min() - 1);
        extract_min();
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    static size_t get_heap_number(Node* node) {
        while (node->parent != nullptr) {
            node = node->parent;
        }
        return node->number;
    }

};


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    size_t n, m;
    std::cin >> n >> m;

    std::vector<BinomialHeap> heaps;

    for (size_t i = 0; i < n; ++i) {
        heaps.emplace_back(i);
    }

    int index;
    int idx, v;
    size_t a, b;
    for (size_t i = 0; i < m; ++i) {

        std::cin >> index;

        switch (index) {

            case 0:
                // insert a v
                std::cin >> a >> v;
                heaps[0].nodes.push_back(heaps[a - 1].insert(v, heaps[0].nodes.size()));
                break;

            case 1:
                // merge a b
                // a is empty after merge
                std::cin >> a >> b;
                heaps[b - 1].merge(heaps[a - 1]);
                break;

            case 2:
                // delete idx
                std::cin >> idx;
                --idx;
                heaps[BinomialHeap::get_heap_number(heaps[0].nodes[idx])].erase(heaps[0].nodes[idx]);
                break;

            case 3:
                // assign idx v
                std::cin >> idx >> v;
                --idx;
                heaps[BinomialHeap::get_heap_number(heaps[0].nodes[idx])].assign(heaps[0].nodes[idx], v);
                break;

            case 4:
                // get_min a
                std::cin >> a;
                std::cout << heaps[a - 1].get_min() << '\n';
                break;

            case 5:
                // extract_min a
                std::cin >> a;
                heaps[a - 1].extract_min();
                break;

            default:
                break;

        }

    }

}