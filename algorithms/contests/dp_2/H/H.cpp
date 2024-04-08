// max matching on arbitrary graph for O(n * 2^n) :)
// no blossom algorithm or something polynomial


#include <algorithm>
#include <iostream>
#include <vector>


bool bit(int mask, int i) {
    return (mask >> i) & 1;
}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<std::vector<bool>> adjacency_matrix(n, std::vector<bool>(n));
    std::vector<int> matching_size(1 << n, 0);

    char c;
    for (auto& row : adjacency_matrix) {
        for (int i = 0; i < n; ++i) {
            std::cin >> c;
            row[i] = c == 'Y';
        }
    }

    for (int mask = 0; mask < 1 << n; ++mask) {
        for (int i = 0; i < n; ++i) {
            if (bit(mask, i)) {
                matching_size[mask] = matching_size[mask ^ (1 << i)];
                for (int j = i + 1; j < n; ++j) {
                    if (bit(mask, j) && adjacency_matrix[i][j]) {
                        matching_size[mask] = std::max(matching_size[mask],
                                                       matching_size[mask ^ (1 << i) ^ (1 << j)] + 1);
                    }
                }
                break;
            }
        }
    }

    std::cout << *std::max_element(matching_size.begin(), matching_size.end()) * 2 << '\n';

}