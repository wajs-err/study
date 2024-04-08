#include <iostream>
#include <vector>
#include <string>
#include <cmath>


class BigInteger;
class Rational;
bool operator!=(const BigInteger&, const BigInteger&);
bool operator<=(const BigInteger&, const BigInteger&);
BigInteger operator-(const BigInteger&, const BigInteger&);
BigInteger operator+(const BigInteger&, const BigInteger&);
BigInteger operator*(const BigInteger&, const BigInteger&);
BigInteger operator/(const BigInteger&, const BigInteger&);


class BigInteger {

    friend std::istream& operator>>(std::istream&, BigInteger&);
    friend bool operator<(const BigInteger&, const BigInteger&);
    friend bool operator==(const BigInteger&, const BigInteger&);
    friend BigInteger operator""_bi(const char*);
    friend Rational;

public:

    BigInteger(): is_negative(false), digits(std::vector<int> (0)) {}

    BigInteger(int num): is_negative(num < 0), digits(std::vector<int> (0)) {

        num = std::abs(num);

        do {
            digits.push_back(num % BASE);
            num /= BASE;
        } while (num != 0);

    }

    BigInteger(const BigInteger& num) = default;

    BigInteger(const std::string& str): is_negative(str[0] == '-'), digits(std::vector<int> (0)) {

        std::string s;
        if (str[0] == '-' || str[0] == '+') {
            s = str.substr(1, str.size() - 1);
        } else {
            s = str;
        }

        for (size_t i = s.size(); static_cast<int>(i) > 0; i -= BASE_EXPONENT) {

            if (i < BASE_EXPONENT) {
                digits.push_back( stoi(s.substr(0, i)) );
            } else {
                digits.push_back( stoi(s.substr(i - BASE_EXPONENT, BASE_EXPONENT)) );
            }

        }

        if (size() == 1 && digits[0] == 0) {
            is_negative = false;
        }

    }

    BigInteger& operator=(const BigInteger& num) = default;

    ~BigInteger() = default;

    BigInteger& operator++() {

        if (*this == -1) {

            *this = 0;
            is_negative = false;

        } else if (is_negative) {

            int carry;
            size_t i = 0;

            do {
                --digits[i];
                carry = digits[i] >= 0 ? 0 : 1;
                if (digits[i] < 0) {
                    digits[i] += BASE;
                }
                ++i;
            } while (carry != 0);

            removeLeadingZeros();

        } else {

            int carry;
            size_t i = 0;

            do {
                if (i == size()) {
                    digits.push_back(0);
                }
                ++digits[i];
                carry = digits[i] / BASE;
                digits[i] %= BASE;
                ++i;
            } while (carry != 0);

        }

        return *this;

    }

    BigInteger& operator--() {

        if (*this == 0) {

            *this = -1;
            is_negative = true;

        } else if (is_negative) {

            int carry;
            size_t i = 0;

            do {
                if (i == size()) {
                    digits.push_back(0);
                }
                ++digits[i];
                carry = digits[i] / BASE;
                digits[i] %= BASE;
                ++i;
            } while (carry != 0);

        } else {

            int carry;
            size_t i = 0;

            do {
                --digits[i];
                carry = digits[i] >= 0 ? 0 : 1;
                if (digits[i] < 0) {
                    digits[i] += BASE;
                }
                ++i;
            } while (carry != 0);

            removeLeadingZeros();

        }

        return *this;

    }

    BigInteger operator++(int) {
        BigInteger ret = *this;
        ++*this;
        return ret;
    }

    BigInteger operator--(int) {
        BigInteger ret = *this;
        --*this;
        return ret;
    }

    BigInteger operator-() const {
        BigInteger ret = *this;
        if (ret != 0) {
            ret.is_negative = !ret.is_negative;
        }
        return ret;
    }

    explicit operator bool() const {
        return !(*this == 0);
    }

    std::string toString() const {

        std::string ret;

        if (is_negative) {
            ret.push_back('-');
        }

        ret += std::to_string(digits.back());
        for (size_t i = size() - 2; static_cast<int>(i) >= 0; --i) {

            for (size_t j = 0; j < BASE_EXPONENT - std::to_string(digits[i]).size(); ++j) {
                ret += '0';
            }

            ret += std::to_string(digits[i]);

        }

        return ret;

    }

    BigInteger abs() const {
        return is_negative ? -*this : *this;
    }

    // multiplies BigInteger by BASE in power n
    BigInteger shiftLeft(size_t shift_power = 1) {

        BigInteger tmp;
        tmp.digits.resize(size() + shift_power);

        for (size_t i = 0; i < size(); ++i) {
            tmp[i + shift_power] = digits[i];
        }

        for (size_t i = 0; i < shift_power; ++i) {
            tmp[i] = 0;
        }

        tmp.removeLeadingZeros();

        return tmp;

    }

    BigInteger& operator+=(const BigInteger& rhs) {

        if (is_negative == rhs.is_negative) {

            int carry = 0;
            for (size_t i = 0; i < std::max(size(), rhs.size()) || carry != 0; ++i) {

                if (i == size()) {
                    digits.push_back(0);
                }

                digits[i] += carry + (i < rhs.size() ? rhs[i] : 0);
                carry = digits[i] / BigInteger::BASE;
                digits[i] %= BigInteger::BASE;

            }

            return *this;

        }

        if (is_negative && !rhs.is_negative) {
            return *this = rhs - -*this;
        }

        if (!is_negative && rhs.is_negative) {
            return *this -= -rhs;
        }

        return *this;

    }

    BigInteger& operator-=(const BigInteger& rhs) {

        if (!is_negative && !rhs.is_negative) {

            BigInteger max = std::max(*this, rhs);
            BigInteger min = std::min(*this, rhs);

            int carry = 0;
            for (size_t i = 0; i < std::max(max.size(), min.size()) || carry != 0; ++i) {

                max[i] -= carry + (i < min.size() ? min[i] : 0);
                carry = max[i] < 0 ? 1 : 0;
                if (max[i] < 0) {
                    max[i] += BigInteger::BASE;
                }

            }

            max.removeLeadingZeros();

            max.is_negative = *this < rhs;

            return *this = max;

        } else {
            return *this += -rhs;
        }

    }

    BigInteger& operator*=(const BigInteger& rhs) {

        BigInteger ret;
        ret.digits.resize(size() + rhs.size());

        ret.is_negative = is_negative != rhs.is_negative; // TODO

        for (size_t i = 0; i < size(); ++i) {

            long long carry = 0;
            for (size_t j = 0; j < rhs.size() || carry != 0; ++j) {

                long long mult =
                        carry + static_cast<long long>(ret[i + j]) +
                        static_cast<long long>(digits[i]) * static_cast<long long>(j < rhs.size() ? rhs[j] : 0);


                carry = static_cast<int>(mult / BigInteger::BASE);
                ret[i + j] = static_cast<int>(mult % BigInteger::BASE);

            }

        }

        ret.removeLeadingZeros();

        return *this = ret;


    }

    BigInteger& operator/=(const BigInteger& rhs) {

        BigInteger res = 0;
        BigInteger rhs_abs = rhs.abs();

        if (abs() < rhs_abs) {
            return *this = res;
        }

        size_t current_pos = size() - 1;
        size_t divisor_size = rhs_abs.size();

        BigInteger part = partOf(current_pos, divisor_size);
        current_pos -= divisor_size - 1;

        do {

            while (part < rhs_abs && current_pos > 0) {
                --current_pos;
                part = part.shiftLeft() + digits[current_pos];
                res = res.shiftLeft();
            }

            // binary search for result of dividing the copy to b_abs
            int left = 0;
            int right = BigInteger::BASE;

            while (right - left > 1) {

                int middle = (left + right) / 2;

                if (middle * rhs_abs <= part) {
                    left = middle;
                } else {
                    right = middle;
                }

            }

            res = res.shiftLeft() + left;
            part -= left * rhs_abs;

            if (current_pos == 0) break;
            --current_pos;

            part = part.shiftLeft() + digits[current_pos];

        } while (static_cast<int>(current_pos) >= 0);

        res.is_negative = is_negative != rhs.is_negative;

        return *this = res;

    }

    BigInteger& operator%=(const BigInteger& rhs) {
        return *this -= *this / rhs * rhs;
    }

private:

    // BASE can be changed to any power of 10, such that
    // 2 * BASE fits in int
    // BASE^2 fits in long long
    static const int BASE = 1'000'000'000;
    static const int BASE_EXPONENT = 9;
    bool is_negative;
    std::vector<int> digits;

    int& operator[](size_t index) {
        return digits[index];
    }

    int operator[](size_t index) const {
        return digits[index];
    }

    size_t size() const {
        return digits.size();
    }

    void removeLeadingZeros() {

        while (size() >= 1 && digits.back() == 0) {
            digits.pop_back();
        }

        if (size() == 0) {
            digits.push_back(0);
        }

    }

    // copies part of digits vector of BigInteger
    BigInteger partOf(size_t pos, size_t count) const {

        BigInteger ret;

        for (size_t i = pos - count + 1; i <= pos; ++i) {
            ret.digits.push_back(digits[i]);
        }

        ret.removeLeadingZeros();

        return ret;

    }

};

BigInteger operator""_bi(const char* str) {
    const std::string s(str);
    BigInteger ret(s);
    return ret;
}

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ret = lhs;
    return ret += rhs;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ret = lhs;
    return ret -= rhs;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ret = lhs;
    return ret *= rhs;
}

BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ret = lhs;
    return ret /= rhs;
}

BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ret = lhs;
    return ret %= rhs;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& num) {
    return out << num.toString();
}

std::istream& operator>>(std::istream& in, BigInteger& num) {

    std::string input;
    in >> input;
    num = input;
    return in;

}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {

    if (lhs.is_negative != rhs.is_negative || lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;

}

bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs == rhs);
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {

    if (lhs.is_negative != rhs.is_negative) {
        return lhs.is_negative;
    }

    if (lhs.size() != rhs.size()) {
        return (lhs.size() < rhs.size() && !lhs.is_negative) || (lhs.size() > rhs.size() && lhs.is_negative);
    }

    for (size_t i = lhs.size() - 1; static_cast<int>(i) >= 0; --i) {
        if (lhs[i] != rhs[i]) {
            return (lhs[i] < rhs[i] && !lhs.is_negative) || (lhs[i] > rhs[i] && lhs.is_negative);
        }
    }

    return false;

}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
    return rhs < lhs;
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs < rhs);
}


class Rational {

    friend bool operator==(const Rational&, const Rational&);
    friend bool operator<(const Rational&, const Rational&);
    friend std::istream& operator>>(std::istream&, Rational&);

public:

    Rational() = default;

    Rational(const Rational& num) = default;

    Rational(const BigInteger& num): numerator(num), denominator(1) {}

    Rational(int num): numerator(num), denominator(1) {}

    Rational& operator=(const Rational& num) = default;

    Rational operator-() const {
        Rational ret = *this;
        if (numerator != 0) {
            ret.numerator.is_negative = !ret.numerator.is_negative;
        }
        return ret;
    }

    std::string toString() const {

        reduce();

        if (denominator != 1) {
            return numerator.toString() + '/' + denominator.toString();
        } else {
            return numerator.toString();
        }

    }

    std::string asDecimal(size_t precision = 0) const {

        reduce();

        std::string ret;
        if (numerator.is_negative) {
            ret = '-';
        }
        ret += (numerator.abs() / denominator).toString();

        Rational tmp = abs();
        tmp.numerator %= tmp.denominator;
        tmp.numerator = tmp.numerator.shiftLeft(
                (precision + BigInteger::BASE_EXPONENT - 1) / BigInteger::BASE_EXPONENT);

        if (precision != 0) {

            ret.push_back('.');
            size_t dot_position = ret.size() - 1;

            for (size_t i = 0; i < (precision + BigInteger::BASE_EXPONENT - 1) /
                                    BigInteger::BASE_EXPONENT * BigInteger::BASE_EXPONENT -
                                    (tmp.numerator / tmp.denominator).toString().size(); ++i) {

                ret += '0';

            }

            ret += (tmp.numerator / tmp.denominator).toString();
            size_t new_digit = round(ret, dot_position + precision + 1);
            ret.erase(dot_position + precision + new_digit + 1, ret.size() - dot_position - precision);

        } else {

            if ((tmp.numerator / tmp.denominator) >= 5) {
                round(ret, ret.size() - 1);
            }

        }

        return ret;

    }

    explicit operator double() const {

        reduce();

        const size_t double_max_exponent = 324;
        std::string s = abs().asDecimal(double_max_exponent);
        double ret = 0;

        size_t dot_position = s.find('.');
        s.erase(dot_position, 1);

        for (size_t i = 0; i < s.size() - 1; ++i) {
            ret += static_cast<double>(s[i] - '0') * pow(10, static_cast<int>(dot_position - i - 1));
        }

        if (numerator.is_negative) {
            ret *= -1;
        }

        return ret;

    }

    Rational abs() const {
        return numerator.is_negative ? -*this : *this;
    }

    Rational& operator+=(const Rational& rhs) {
        numerator *= rhs.denominator;
        numerator += rhs.numerator * denominator;
        denominator *= rhs.denominator;
        reduce();
        return *this;
    }

    Rational& operator-=(const Rational& rhs) {
        return *this += -rhs;
    }

    Rational& operator*=(const Rational& rhs) {

        numerator *= rhs.numerator;
        denominator *= rhs.denominator;
        return *this;

    }

    Rational& operator/=(const Rational& rhs) {

        Rational tmp = *this;
        tmp.numerator *= rhs.denominator;
        tmp.denominator *= rhs.numerator;
        if (rhs.numerator.is_negative) {
            tmp.denominator.is_negative = !tmp.denominator.is_negative;
            tmp.numerator.is_negative = !tmp.numerator.is_negative;
        }
        reduce();
        return *this = tmp;

    }

private:

    mutable BigInteger numerator;
    mutable BigInteger denominator;

    void reduce() const {

        Rational tmp = abs();

        // Euclid algorithm
        while (tmp.numerator != 0 && tmp.denominator != 0) {
            if (tmp.numerator < tmp.denominator) {
                tmp.denominator %= tmp.numerator;
            } else {
                tmp.numerator %= tmp.denominator;
            }
        }

        numerator /= std::max(tmp.numerator, tmp.denominator);
        denominator /= std::max(tmp.numerator, tmp.denominator);

    }

    // rounds posth digit after the point
    // s is string representation of rounded number
    // returns 1 if number of digits increased, 0 otherwise
    static size_t round(std::string& s, size_t pos) {

        if (s[pos] >= '5') {

            s[pos] = '0';

            if (static_cast<int>(pos - 1) >= 0 && s[pos - 1] == '.') {
                --pos;
            }

            if (pos == 0) {
                s = '1' + s;
                return 1;
            }

            if (s[pos - 1] != '9') {
                ++s[pos - 1];
            } else {
                return round(s, pos - 1);
            }

        }

        return 0;

    }

};

Rational operator+(const Rational& lhs, const Rational& rhs) {
    Rational ret = lhs;
    return ret += rhs;
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
    Rational ret = lhs;
    return ret -= rhs;
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
    Rational ret = lhs;
    return ret *= rhs;
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
    Rational ret = lhs;
    return ret /= rhs;
}

std::ostream& operator<<(std::ostream& out, const Rational& num) {
    return out << num.toString();
}

std::istream& operator>>(std::istream& in, Rational& num) {
    return in >> num.numerator;
}

bool operator==(const Rational& lhs, const Rational& rhs) {
    lhs.reduce();
    rhs.reduce();
    return lhs.numerator == rhs.numerator &&
           lhs.denominator == rhs.denominator;
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Rational& lhs, const Rational& rhs) {
    return lhs.numerator * rhs.denominator < rhs.numerator * rhs.denominator;
}

bool operator>(const Rational& lhs, const Rational& rhs) {
    return rhs < lhs;
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
    return !(lhs < rhs);
}
