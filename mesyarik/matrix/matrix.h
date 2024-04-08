#include "biginteger.h"


using std::vector;


namespace primeness {

    template <int N, int M>
    struct Divide {
        static const int value = Divide<N / 2, M + 1>::value;
    };

    template <int M>
    struct Divide<1, M> {
        static const int value = M;
    };

    template <int N>
    struct BinaryLog {
        static const int value = Divide<N, 1>::value;
    };

    template <int N, int M>
    struct Multiply {
        static const int value = Multiply<N * 2, M - 1>::value;
    };

    template <int N>
    struct Multiply<N, 0> {
        static const int value = N;
    };

    template <int N>
    struct BinaryPow {
        static const int value = Multiply<1, N>::value;
    };

    template <int N, int M>
    struct IsDivided {
        static const bool value = N == 2 || (N % M != 0 && IsDivided<N, M - 1>::value);
    };

    template <int N>
    struct IsDivided<N, 1> {
        static const bool value = true;
    };

    template <int N>
    struct IsPrime {
        static const bool value =
                IsDivided<N, BinaryPow<(BinaryLog<N>::value + 1) / 2>::value>::value;
    };

}


template <size_t N>
class Residue {

public:

    Residue() = default;

    explicit Residue(int init)
            : element((static_cast<int>(N) + init % static_cast<int>(N)) % static_cast<int>(N)) {}

    Residue(const Residue& init) = default;

    ~Residue() = default;

    Residue& operator=(const Residue& init) = default;

    explicit operator int() const {
        return element;
    }

    Residue& operator+=(const Residue& rhs) {
        (element += rhs.element) %= static_cast<int>(N);
        return *this;
    }

    Residue& operator-=(const Residue& rhs) {
        return *this += static_cast<Residue<N>>(static_cast<int>(N) - rhs.element);
    }

    Residue& operator*=(const Residue& rhs) {
        (element *= rhs.element) %= static_cast<int>(N);
        return *this;
    }

    Residue& operator/=(const Residue& rhs) {

        static_assert(primeness::IsPrime<N>::value, "Dividing is only defined by prime modula");

        int power = N - 2;
        auto inverse = static_cast<Residue>(1);
        Residue tmp = rhs;

        while (power > 0) {

            if (power & 1) {
                inverse *= static_cast<Residue>(tmp);
            }

            tmp *= tmp;
            power >>= 1;

        }

        return *this *= Residue{inverse};

    } // iff N is prime

    bool operator==(const Residue& rhs) const {
        return rhs.element == element;
    }

    bool operator!=(const Residue& rhs) const {
        return !(rhs == *this);
    }

private:

    int element;

};

template <size_t N>
Residue<N> operator+(const Residue<N>& lhs, const Residue<N>& rhs) {
    Residue ret(lhs);
    return ret += rhs;
}

template <size_t N>
Residue<N> operator-(const Residue<N>& lhs, const Residue<N>& rhs) {
    Residue ret(lhs);
    return ret -= rhs;
}

template <size_t N>
Residue<N> operator*(const Residue<N>& lhs, const Residue<N>& rhs) {
    Residue ret(lhs);
    return ret *= rhs;
}

template <size_t N>
Residue<N> operator/(const Residue<N>& lhs, const Residue<N>& rhs) {
    Residue ret(lhs);
    return ret /= rhs;
} // iff N is prime

template <size_t N>
std::ostream& operator<<(std::ostream& out, const Residue<N>& num) {
    return out << static_cast<int>(num);
}


template <size_t M, size_t N, typename Field = Rational>
class Matrix {

public:

    // returns unit matrix if it is square, zero matrix otherwise
    Matrix(): matrix(vector<vector<Field>>(M, vector<Field>(N, static_cast<Field>(0)))) {
        if (N == M) {
            for (size_t i = 0; i < N; ++i) {
                matrix[i][i] = static_cast<Field>(1);
            }
        }
    }

    Matrix(const Matrix& init): matrix(init.matrix) {}

    explicit Matrix(const Field& init): matrix() {
        matrix.resize(M);
        for (size_t i = 0; i < M; ++i) {
            matrix[i].resize(N, static_cast<Field>(0));
            matrix[i][i] = init;
        }
    }

    explicit Matrix(const vector<vector<Field>>& init): matrix(init) {}

    explicit Matrix(const vector<vector<int>>& init): matrix() {
        matrix.resize(M);
        for (size_t i = 0; i < M; ++i) {
            matrix[i].resize(N);
            for (size_t j = 0; j < N; ++j) {
                matrix[i][j] = static_cast<Field>(init[i][j]);
            }
        }
    }

    Matrix(std::initializer_list<std::vector<Field>> init): matrix(init) {}

    Matrix(std::initializer_list<std::vector<int>> init): matrix() {

        matrix.resize(M);

        for (auto it = init.begin(); it < init.end(); ++it) {
            matrix[it - init.begin()].resize(N);
            for (size_t j = 0; j < N; ++j) {
                matrix[it - init.begin()][j] = static_cast<Field>((*it)[j]);
            }
        }

    }

    ~Matrix() = default;

    Matrix& operator=(const Matrix& rhs) = default;

    vector<Field>& operator[](size_t index) {
        return matrix[index];
    }

    vector<Field> operator[](size_t index) const {
        return matrix[index];
    }

    Matrix& operator+=(const Matrix& rhs) {

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                matrix[i][j] += rhs[i][j];
            }
        }

        return *this;

    }

    Matrix& operator-=(const Matrix& rhs) {

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                matrix[i][j] -= rhs[i][j];
            }
        }

        return *this;

    }

    Matrix& operator*=(const Field& rhs) {

        for (auto& row : matrix) {
            for (auto& el : row) {
                el *= rhs;
            }
        }

        return *this;

    }

    template <size_t K>
    Matrix<M, K, Field> operator*(const Matrix<N, K, Field>& rhs) const {

        Matrix<M, K, Field> ret(static_cast<Field>(0));

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < K; ++j) {
                for (size_t k = 0; k < N; ++k) {
                    ret[i][j] += matrix[i][k] * rhs[k][j];
                }
            }
        }

        return ret;

    }

    vector<Field> getRow(size_t index) const {
        return matrix[index];
    }

    vector<Field> getColumn(size_t index) const {

        vector<Field> ret;

        for (auto& row : matrix) {
            ret.push_back(row[index]);
        }

        return ret;

    }

    Matrix<N, M, Field> transposed() const {

        Matrix<N, M, Field> ret;
        for (size_t i = 0; i < N; ++i) {
            ret[i] = getColumn(i);
        }
        return ret;

    }

    size_t rank() const {

        Matrix<M, N, Field> echelon_matrix(*this);
        Matrix<M, N, Field> tmp;
        echelon_matrix.makeEchelon(tmp);

        size_t ret = 0;

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                if (echelon_matrix.matrix[i][j] != static_cast<Field>(0)) {
                    ++ret;
                    break;
                }
            }
        }

        return ret;

    }

    Field trace() const {

        static_assert(M == N, "Trace is only defined for square matrices");

        auto ret = static_cast<Field>(0);

        for (size_t i = 0; i < N; ++i) {
            ret += matrix[i][i];
        }

        return ret;

    }

    Field det() const {

        static_assert(M == N, "Determinant is only defined for square matrices");

        Matrix triangular_matrix(*this);
        Matrix tmp;
        triangular_matrix.makeEchelon(tmp);

        auto ret = static_cast<Field>(1);

        for (size_t i = 0; i < N; ++i) {
            ret *= triangular_matrix[i][i];
        }

        return ret;

    }

    void invert() {

        static_assert(M == N, "Inverse matrix is only defined for square matrices");

        Matrix<N, N, Field> tmp(*this);
        Matrix<N, N, Field> inverse_matrix;

        tmp.makeEchelon(inverse_matrix);
        tmp.makeDiagonal(inverse_matrix);
        tmp.makeUnit(inverse_matrix);

        *this = inverse_matrix;

    }

    Matrix inverted() const {
        Matrix ret(*this);
        ret.invert();
        return ret;
    }

    Matrix& operator*=(const Matrix& rhs) {
        static_assert(M == N, "operator*= is only defined for square matrices");
        return *this = *this * rhs;
    }

private:

    // makes matrix echelon
    void makeEchelon(Matrix& rhs = Matrix<M, N, Field>()) {

        Matrix<M, 2 * N, Field> tmp;
        mergeMatrices(tmp, *this, rhs);

        for (size_t i = 0; i < M - 1; ++i) {

            if (tmp[i][i] == static_cast<Field>(0)) {

                for (size_t j = i + 1; j < M; ++j) {
                    if (tmp[j][i] != static_cast<Field>(0)) {
                        for (size_t k = 0; k < 2 * N; ++k) {
                            tmp[i][k] += tmp[j][k];
                        }
                        break;
                    }
                }

                if (tmp[i][i] == static_cast<Field>(0)) {
                    continue;
                }

            }

            for (size_t j = i + 1; j < M; ++j) {
                Field ratio = tmp[j][i] / tmp[i][i];
                for (size_t k = 0; k < 2 * N; ++k) {
                    tmp[j][k] -= ratio * tmp[i][k];
                }
            }

        }

        splitMatrices(tmp, *this, rhs);

    }

    // makes upper triangular matrix diagonal
    void makeDiagonal(Matrix& rhs = Matrix<M, N, Field>()) {

        static_assert(M == N, "Only square matrix can be diagonal");

        Matrix<M, 2 * N, Field> tmp;
        mergeMatrices(tmp, *this, rhs);

        for (size_t i = N - 1; i > 0; --i) {

            for (size_t j = i - 1; static_cast<int>(j) >= 0; --j) {
                Field ratio = tmp[j][i] / tmp[i][i];
                for (size_t k = 0; k < 2 * N; ++k) {
                    tmp[j][k] -= ratio * tmp[i][k];
                }
            }

        }

        splitMatrices(tmp, *this, rhs);

    }

    // makes diagonal matrix unit
    void makeUnit(Matrix& rhs = Matrix<M, N, Field>()) {

        static_assert(M == N, "Only square matrix can be unit");

        Matrix<M, 2 * N, Field> tmp;
        mergeMatrices(tmp, *this, rhs);

        for (size_t i = 0; i < N; ++i) {
            if (tmp[i][i] != static_cast<Field>(1)) {
                for (size_t k = N; k < 2 * N; ++k) {
                    tmp[i][k] /= tmp[i][i];
                }
            }
        }

        splitMatrices(tmp, *this, rhs);

    }

    // dual = (lhs | rhs)
    static void mergeMatrices(Matrix<M, 2 * N, Field>& dual,
                              Matrix<M, N, Field>& lhs,
                              Matrix<M, N, Field>& rhs) {

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                dual[i][j] = lhs[i][j];
                dual[i][j + N] = rhs[i][j];
            }
        }

    }

    // dual = (lhs | rhs)
    static void splitMatrices(Matrix<M, 2 * N, Field>& dual,
                              Matrix<M, N, Field>& lhs,
                              Matrix<M, N, Field>& rhs) {

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                lhs[i][j] = dual[i][j];
            }
        }

        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                rhs[i][j] = dual[i][j + N];
            }
        }

    }

    vector<vector<Field>> matrix;

};

template <size_t M, size_t N, typename Field = Rational>
bool operator==(const Matrix<M, N, Field>& lhs, const Matrix<M, N, Field>& rhs) {

    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (lhs[i][j] != rhs[i][j]) {
                return false;
            }
        }
    }

    return true;

}

template <size_t M, size_t N, typename Field = Rational>
bool operator!=(const Matrix<M, N, Field>& lhs, const Matrix<M, N, Field>& rhs) {
    return !(lhs == rhs);
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& lhs, const Matrix<M, N, Field>& rhs) {
    Matrix<M, N, Field> ret = lhs;
    return ret += rhs;
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& lhs, const Matrix<M, N, Field>& rhs) {
    Matrix<M, N, Field> ret = lhs;
    return ret -= rhs;
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator*(Field lhs, const Matrix<M, N, Field>& rhs) {
    Matrix<M, N, Field> ret(rhs);
    return ret *= lhs;
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator*(const Matrix<M, N, Field>& lhs, Field rhs) {
    Matrix<M, N, Field> ret(lhs);
    return ret *= rhs;
}

template <size_t M, size_t N, typename Field = Rational>
std::ostream& operator<<(std::ostream& out, Matrix<M, N, Field> rhs) {
    for (size_t i = 0; i < M; ++i) {
        for (auto& el : rhs.getRow(i)) {
            out << el << ' ';
        }
        out << '\n';
    }
    return out << '\n';
}


template <size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;