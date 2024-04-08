#include <iostream>
#include <vector>


using std::vector;
using Matrix = vector<vector<int64_t>>;
constexpr int64_t mod = 1'000'000'000 + 7;
constexpr int64_t half_mod = (mod + 1) / 2; // is needed to divide by 2 in ring modulo mod


// global variables are needed to avoid TL
vector<vector<int64_t>> answer(2, vector<int64_t>(2, 0));
void multiply(Matrix& lhs, const Matrix& rhs) {

    auto size = lhs.size();

    answer[0][0] = answer[0][1] = answer[1][0] = answer[1][1] = 0;

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            for (size_t k = 0; k < size; ++k) {
                (answer[i][j] += lhs[i][k] * rhs[k][j] % mod) %= mod;
            }
        }
    }

    lhs = answer;

}


// global variables are needed to avoid TL
std::vector<std::vector<int64_t>> M(2, std::vector<int64_t>(2, 0));
void power(Matrix& lhs, int64_t power) {

    auto size = lhs.size();

    M[0][1] = 0;
    M[1][0] = 0;
    for (size_t i = 0; i < size; ++i) {
        M[i][i] = 1;
    }

    while (power != 0) {

        if (power % 2 == 1) {
            multiply(M, lhs);
        }

        multiply(lhs, lhs);
        power >>= 1;

    }

    lhs = M;

}


int64_t power(int64_t x, int64_t power) {

    int64_t mult = x;
    int64_t res = 1;

    while (power != 0) {

        if (power % 2 == 1) {
            (res *= mult) %= mod;
        }

        (mult *= mult) %= mod;
        power >>= 1;

    }

    return res;

}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int k;
    std::cin >> k;

    std::vector<int64_t> a(k);
    for (auto& el : a) {
        std::cin >> el;
    }

    int count_ones = 0;
    int64_t val = 0;
    std::for_each(a.begin(), a.end(), [&count_ones, &val](const int64_t& el){
        if (el == 1) {
            ++count_ones;
        }
        if (el != 1) {
            val = el;
        }
    });

    if (count_ones == k) {
        std::cout << "0/1\n";
        return 0;
    }
    if (val == 2 && count_ones == k - 1) {
        std::cout << "1/2\n";
        return 0;
    }
    if (val == 3 && count_ones == k - 1) {
        std::cout << "1/4\n";
        return 0;
    }

    Matrix M = {{-1, 1},
                {0, 2}};

    // M^-1 = {{-1, 1/2},
    //         {0, 1/2}};

    for (auto& el : a) {
        if (el != 1) {
            power(M, el);
        }
    }

    // M^(\prod a) * M^-3
    Matrix copy = M;

    copy[0][0] = -M[0][0];
    copy[0][1] = (M[0][0] + M[0][1]) * half_mod % mod;
    copy[1][0] = -M[1][0];
    copy[1][1] = (M[1][0] + M[1][1]) * half_mod % mod;

    M[0][0] = -copy[0][0];
    M[0][1] = (copy[0][0] + copy[0][1]) * half_mod % mod;
    M[1][0] = -copy[1][0];
    M[1][1] = (copy[1][0] + copy[1][1]) * half_mod % mod;

    copy[0][0] = -M[0][0];
    copy[0][1] = (M[0][0] + M[0][1]) * half_mod % mod;
    copy[1][0] = -M[1][0];
    copy[1][1] = (M[1][0] + M[1][1]) * half_mod % mod;

    M = copy;

    int64_t p = (M[0][0] + M[0][1] * 4) % mod;
    if (p < 0) {
        p += mod;
    }

    int64_t q = 2;

    for (auto& el : a) {
        if (el != 1) {
            q = power(q, el);
        }
    }

    (q *= half_mod) %= mod;
    std::cout << p << '/' << q << '\n';

    return 0;

}