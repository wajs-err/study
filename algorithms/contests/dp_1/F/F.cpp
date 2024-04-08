// O(n^3), n <= 2000 in 2 seconds :)


#include <iostream>
#include <deque>


uint64_t P(int n, int k, int start) {
    if (n == 0) {
        return 1;
    }
    if (k == 0) {
        return 0;
    }
    if (k > n) {
        k = n;
    }
    if (k <= start || n <= start) {
        return 0;
    }
    return P(n, k - 1, start) + P(n - k, k - 1, start);
}


int main() {

    int n;
    std::cin >> n;
    ++n;

    std::deque deque = {0, 1};
    int sum = 1;
    while (sum < n - 1) {
        deque.push_back(deque.back() + 1);
        sum += deque.back();
        deque.pop_front();
        if (deque.front() <= deque.back() / 2) {
            sum -= deque.front();
            deque.pop_front();
        }
        deque.push_front(0);
    }

    int64_t res = 0;
    for (int max = deque.back(); max < n; ++max) {

        std::vector<std::vector<int64_t>> dp(n - deque.back(), std::vector<int64_t>(deque.size(), 0));
        dp[0][0] = 1;

        for (int i = 0; i < n - deque.back(); ++i) {
            for (int k = 1; k < deque.size() - 1; ++k) {
                dp[i][k] += dp[i][k - 1];
                if (i - deque[k] >= 0) {
                    dp[i][k] += dp[i - deque[k]][k - 1];
                }
            }
        }

        res += dp.back()[deque.size() - 2];

        deque.push_back(deque.back() + 1);
        deque.pop_front();
        if (deque.front() <= deque.back() / 2) {
            deque.pop_front();
        }
        deque.push_front(0);

    }

    std::cout << res << '\n';

    // correct, but slow
//    --n;
//    res = 0;
//    for (int i = 1; i <= n; ++i) {
//        res += P(n - i, i - 1, i / 2);
//    }
//    std::cout << res << '\n';

}