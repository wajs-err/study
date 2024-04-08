// This solution is O(n log n) with coordinate compression
// n <= 10^5


#include <iostream>
#include <vector>


class DynamicSegmentTree {

public:

    DynamicSegmentTree() : nodes(1, {0, 0, 100'000, -1, -1}) {}

    void insert(int x, int val) {
        insert(x, val, 0);
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

    void insert(int x, int val, size_t index) {

        nodes[index].sum += val;

        if (nodes[index].left_bound == nodes[index].right_bound) {
            return;
        }

        int m = (nodes[index].left_bound + nodes[index].right_bound) / 2;

        if (x <= m) {
            if (nodes[index].left_son == -1) {
                nodes[index].left_son = nodes.size();
                nodes.emplace_back(0, nodes[index].left_bound, m, -1, -1);
            }
            insert(x, val, nodes[index].left_son);
            return;
        }

        if (nodes[index].right_son == -1) {
            nodes[index].right_son = nodes.size();
            nodes.emplace_back(0, m + 1, nodes[index].right_bound, -1, -1);
        }
        insert(x, val, nodes[index].right_son);

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

    std::vector<std::pair<char, int>> v(n);
    std::vector<int> xs(n);

    for (size_t i = 0; i < n; ++i) {
        std::cin >> v[i].first >> v[i].second;
        xs[i] = v[i].second;
    }

    std::sort(xs.begin(), xs.end());
    xs.resize(std::unique(xs.begin(), xs.end()) - xs.begin());

    for (auto& [c, x] : v) {

        if (c == '+') {
            t.insert(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin(), x);
            continue;
        }

        std::cout << t.prefix_sum(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin()) << '\n';

    }

}