#include <iostream>
#include <string>
#include <vector>
#include <array>


class SuffixArray {

private:

    std::string string_;
    size_t size_;
    std::vector<int> suffix_array_;

    // sorts all substrings of length 2^(k + 1)
    // suffix_array_ must be sorted permutation of substrings of length 2^k
    void substrings_stable_sort(int k, std::vector<int>& equivalence_class) {

        std::vector<int> new_permutation(size_);
        for (size_t i = 0; i < size_; ++i) {
            new_permutation[i] = (suffix_array_[i] - (1 << k) + size_) % size_;
        }

        std::vector<int> count(size_);
        for (const auto& el : equivalence_class) {
            ++count[el];
        }

        for (size_t i = 1; i < size_; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = size_ - 1; i >= 0; --i) {
            suffix_array_[--count[equivalence_class[new_permutation[i]]]] = new_permutation[i];
        }

        std::vector<int> new_equivalence_class(size_);
        new_equivalence_class[suffix_array_[0]] = 0;
        for (size_t i = 1; i < size_; ++i) {
            new_equivalence_class[suffix_array_[i]] = new_equivalence_class[suffix_array_[i - 1]];
            if (equivalence_class[suffix_array_[i]] != equivalence_class[suffix_array_[i - 1]] ||
                    equivalence_class[suffix_array_[i] + (1 << k)] !=
                    equivalence_class[suffix_array_[i - 1] + (1 << k)]) {
                ++new_equivalence_class[suffix_array_[i]];
            }
        }

        equivalence_class = new_equivalence_class;

    }

    void sort_symbols(std::vector<int>& equivalence_class) {

        std::array<int, 27> count{};
        for (const auto& el : string_) {
            ++count[el - static_cast<char>('a' - 1)];
        }

        for (size_t i = 1; i < 27; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = size_ - 1; i >= 0; --i) {
            suffix_array_[--count[string_[i] - static_cast<char>('a' - 1)]] = i;
        }

        equivalence_class[suffix_array_[0]] = 0;
        for (size_t i = 1; i < size_; ++i) {
            equivalence_class[suffix_array_[i]] = equivalence_class[suffix_array_[i - 1]];
            if (string_[suffix_array_[i]] != string_[suffix_array_[i - 1]]) {
                ++equivalence_class[suffix_array_[i]];
            }
        }

    }

public:

    explicit SuffixArray(const std::string& s)
            : string_(s + static_cast<char>('a' - 1)), size_(string_.size()), suffix_array_(size_) {

        // f: set of substrings of length k -> {1, ..., n}, such that a < b => f(a) < f(b)
        // equivalence_class[i] -- f(s[i]...s[i+k])
        std::vector<int> equivalence_class(size_);
        sort_symbols(equivalence_class);

        int k = 0;
        while (1 << k < size_) {
            substrings_stable_sort(k, equivalence_class);
            ++k;
        }

    }

    int operator[](size_t index) const {
        return suffix_array_[index + 1];
    }

};


int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string s;
    std::cin >> s;

    SuffixArray suffix_array(s);

    for (size_t i = 0; i < s.size(); ++i) {
        std::cout << suffix_array[i] + 1 << ' ';
    }
    std::cout << '\n';

}