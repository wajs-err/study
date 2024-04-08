// non-dp solution for 1 <= n, m <= 1000
// can probably be solved using dp with broken profile for 1 <= n <= 15; 1 <= m <= 100


#include <iostream>
#include <vector>


constexpr int mod = 1'000'000'000 + 7;


int power(int x, int power) {

    int res = 1;

    for (int i = 0; i < power; ++i) {
        (res *= x) %= mod;
    }

    return res;

}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n, m;
    std::cin >> n >> m;

    std::vector<std::vector<char>> map(n, std::vector<char>(m));

    for (auto& row : map) {
        for (auto& el : row) {
            std::cin >> el;
        }
    }

    int pow = 0;

    if (n == 1) {
        for (const auto& el : map[0]) {
            if (el == '.') {
                ++pow;
            }
        }
        std::cout << power(2, pow) << '\n';
        return 0;
    }

    if (m == 1) {
        for (const auto& el : map) {
            if (el[0] == '.') {
                ++pow;
            }
        }
        std::cout << power(2, pow) << '\n';
        return 0;
    }

    std::vector<char> a(m);
    std::vector<char> b(m);
    for (auto i = 0; i < m; ++i) {
        a[i] = i % 2 == 0 ? '+' : '-';
        b[i] = i % 2 != 0 ? '+' : '-';
    }

    bool ok_a, ok_b;
    bool p_1 = true;
    bool p_2 = true;
    bool possible = true;
    for (auto i = 0; i < n; ++i) {

        ok_a = true;
        ok_b = true;

        for (auto j = 0; j < m; ++j) {
            if (map[i][j] == '.') {
                continue;
            }
            if (map[i][j] != a[j]) {
                ok_a = false;
            }
            if (map[i][j] != b[j]) {
                ok_b = false;
            }
        }

        if (ok_a && ok_b) {
            ++pow;
        }

        if (!ok_a && !ok_b) {
            possible = false;
        }

        if (i % 2 == 0) {
            p_1 &= ok_a;
            p_2 &= ok_b;
        } else {
            p_1 &= ok_b;
            p_2 &= ok_a;
        }

    }

    int res = 0;
    if (possible) {
        res += power(2, pow);
    }

    if (p_1) {
        --res;
    }
    if (p_2) {
        --res;
    }
    (res += mod) %= mod;

    pow = 0;
    possible = true;
    a.resize(n);
    b.resize(n);
    for (auto i = 0; i < n; ++i) {
        a[i] = i % 2 == 0 ? '+' : '-';
        b[i] = i % 2 != 0 ? '+' : '-';
    }

    for (auto i = 0; i < m; ++i) {

        ok_a = true;
        ok_b = true;

        for (auto j = 0; j < n; ++j) {
            if (map[j][i] == '.') {
                continue;
            }
            if (map[j][i] != a[j]) {
                ok_a = false;
            }
            if (map[j][i] != b[j]) {
                ok_b = false;
            }
        }

        if (ok_a && ok_b) {
            ++pow;
        }

        if (!ok_a && !ok_b) {
            possible = false;
            break;
        }

    }

    if (possible) {
        (res += power(2, pow)) %= mod;
    }

    std::cout << res << '\n';

    return 0;

}