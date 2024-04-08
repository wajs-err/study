#include <iostream>
#include <vector>


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    size_t floors, eggs, experiments;
    std::cin >> floors >> eggs;

    if (floors == 1) {
        std::cout << 0 << '\n';
        return 0;
    }

    if (eggs == 0) {
        std::cout << -1 << '\n';
        return 0;
    }

    --floors;

    std::vector<size_t> prev(eggs + 1, 1);
    std::vector t = prev;
    experiments = 1;

    while (prev.back() < floors) {
        ++experiments;
        prev[0] = 0;
        for (size_t i = 1; i <= eggs; ++i) {
            t[i] = prev[i] + prev[i - 1] + 1;
        }
        std::swap(prev, t);
    }

    std::cout << experiments << '\n';

}