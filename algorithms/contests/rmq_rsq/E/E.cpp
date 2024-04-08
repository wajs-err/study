// This solution is O(n log r) without coordinate compression
// n <= 10^5, r <= 10^9


#include <iostream>
#include <vector>


class DynamicSegmentTree {

public:

    DynamicSegmentTree() : nodes(1, {0, 0, 1'000'000'000, -1, -1}) {}

    void insert(int x) {
        insert(x, 0);
    }

    uint64_t prefix_sum(int x) {
        return prefix_sum(x, 0);
    }

private:

    struct Node {
        int64_t sum;
        int left_bound;
        int right_bound;
        int left_son;
        int right_son;
        Node(int64_t sum, int left_bound, int right_bound, int left_son, int right_son)
                : sum(sum),
                left_bound(left_bound), right_bound(right_bound),
                left_son(left_son), right_son(right_son) {}
        ~Node() = default;
    };

    std::vector<Node> nodes;

    void insert(int x, size_t index) {

        nodes[index].sum += x;

        if (nodes[index].left_bound == nodes[index].right_bound) {
            return;
        }

        int m = (nodes[index].left_bound + nodes[index].right_bound) / 2;

        if (x <= m) {
            if (nodes[index].left_son == -1) {
                nodes[index].left_son = nodes.size();
                nodes.emplace_back(0, nodes[index].left_bound, m, -1, -1);
            }
            insert(x, nodes[index].left_son);
            return;
        }

        if (nodes[index].right_son == -1) {
            nodes[index].right_son = nodes.size();
            nodes.emplace_back(0, m + 1, nodes[index].right_bound, -1, -1);
        }
        insert(x, nodes[index].right_son);

    }

    uint64_t prefix_sum(int x, size_t index) {

        if (x == nodes[index].right_bound) {
            return nodes[index].sum;
        }

        if (nodes[index].left_son == -1 && nodes[index].right_son == -1) {
            return 0;
        }

        int m = (nodes[index].left_bound + nodes[index].right_bound) / 2;

        if (x > m) {
            return (nodes[index].left_son == -1 ? 0 : nodes[nodes[index].left_son].sum)
                    + (nodes[index].right_son == -1 ? 0 : prefix_sum(x, nodes[index].right_son));
        }

        return nodes[index].left_son == -1 ? 0 : prefix_sum(x, nodes[index].left_son);

    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    DynamicSegmentTree t;

    char c;
    int x;
    for (auto i = 0 ; i < n; ++i) {
        std::cin >> c >> x;
        if (c == '+') {
            t.insert(x);
            continue;
        }
        std::cout << t.prefix_sum(x) << '\n';
    }

}