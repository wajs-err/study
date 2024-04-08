#include <iostream>
#include <vector>
#include <complex>
#include <numbers>
#include <algorithm>


using Complex = std::complex<long double>;
using Polynomial = std::vector<Complex>;


int64_t reverse_bits(int64_t n, int64_t k) {

    int64_t log = std::__lg(k);
    int64_t tmp_1, tmp_2;
    for (int i = 0; i < log / 2; ++i) {
        tmp_1 = (n >> i) & 1;
        n &= ~(1 << i);
        tmp_2 = (n >> (log - 1 - i)) & 1;
        n &= ~(1 << (log - 1 - i));
        n |= tmp_1 << (log - 1 - i);
        n |= tmp_2 << i;
    }

    return n;

}


std::vector<Complex> evaluate_roots(int64_t k) {
    Complex w;
    w = std::polar(1.L, 2 * std::numbers::pi_v<long double> / static_cast<long double>(k));
    std::vector<Complex> roots(k);
    roots[0] = 1;
    for (int64_t i = 1; i < k; ++i) {
        roots[i] = roots[i - 1] * w;
    }
    return roots;
}


void complement(Polynomial& poly, int64_t k) {
    while (poly.size() < k) {
        poly.emplace_back(0.);
    }
}


template <bool IsReverse = false>
void FFT(Polynomial& poly, int64_t k) {

    std::vector<Complex> w = evaluate_roots(k);
    if constexpr (IsReverse) {
        std::reverse(w.begin() + 1, w.end());
    }

    complement(poly, k);

    auto tmp_poly = poly;
    for (int64_t i = 0; i < poly.size(); ++i) {
        tmp_poly[i] = poly[reverse_bits(i, k)];
    }
    poly = tmp_poly;

    Complex tmp;
    for (int64_t j = 1; j < k; j *= 2) {
        for (size_t l = 0; l < poly.size(); l += 2 * j) {
            for (size_t i = l; i < l + j; ++i) {
                tmp = poly[i];
                poly[i] = poly[i] + w[(i - l) * (k / (2 * j))] * poly[i + j];
                poly[i + j] = tmp + w[(i - l) * (k / (2 * j)) + k / 2] * poly[i + j];
            }
        }
    }

}


int64_t power_of_2(int64_t k) {
    int64_t power = 1;
    while (power < k) {
        power <<= 1;
    }
    return power;
}


Polynomial input_polynomial(int64_t n) {
    double input;
    Polynomial poly(n + 1);
    for (auto i = n; i >= 0; --i) {
        std::cin >> input;
        poly[i].real(input);
    }
    return poly;
}


Polynomial multiply_polynomials(Polynomial& poly_1, int64_t degree_1,
                                Polynomial& poly_2, int64_t degree_2) {
    int64_t k = power_of_2(degree_1 + degree_2 + 1);
    FFT(poly_1, k);
    FFT(poly_2, k);
    for (size_t i = 0; i < poly_1.size(); ++i) {
        poly_1[i] *= poly_2[i];
    }
    FFT<true>(poly_1, k);
    for (auto& el : poly_1) {
        el /= k;
    }
    return poly_1;
}


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int64_t degree_1;
    std::cin >> degree_1;
    auto poly_1 = input_polynomial(degree_1);

    int64_t degree_2;
    std::cin >> degree_2;
    auto poly_2 = input_polynomial(degree_2);

    multiply_polynomials(poly_1, degree_1, poly_2, degree_2);

    std::cout << degree_2 + degree_1 << ' ';
    for (int64_t i = degree_2 + degree_1; i >= 0; --i) {
        std::cout << static_cast<int64_t>(std::round(poly_1[i].real())) << ' ';
    }
    std::cout << '\n';

}
