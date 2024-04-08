#include <iostream>
#include <cstring>

class String {

public:

    String(): size(0),
              capacity(min_capacity),
              string(new char[capacity]) {}

    String(const char* str): size(0),
                             capacity(min_capacity),
                             string(new char[capacity]) {

        size_t i = 0;

        while (str[i] != '\0') {
            push_back(str[i++]);
        }

    }

    String(size_t n, char c): size(n),
                              capacity(max(n, min_capacity)),
                              string(new char[capacity]) {
        memset(string, c, n);
    }

    String(const String& str): size(str.size),
                               capacity(max(str.size, min_capacity)),
                               string(new char[capacity]) {

        memcpy(string, str.string, str.size);

    }

    String(char c): size(1),
                    capacity(min_capacity),
                    string(new char[capacity]) {

        string[0] = c;

    }

    String& operator=(const String& str) {

        // copy-and-swap
        String tmp(str);
        std::swap(string, tmp.string);
        std::swap(size, tmp.size);
        std::swap(capacity, tmp.capacity);
        return *this;

    }

    ~String() {
        delete[] string;
    }

    char& operator[](const size_t index) {
        return string[index];
    }

    char operator[](const size_t index) const {
        return string[index];
    }

    size_t length() const {
        return size;
    }

    void push_back(char c) {

        if (size + 1 >= capacity) {

            char* tmp = new char[capacity];
            for (size_t i = 0; i < size; ++i) {
                tmp[i] = string[i];
            }

            delete[] string;
            capacity *= 2;
            string = new char[capacity];

            for (size_t i = 0; i < size; ++i) {
                string[i] = tmp[i];
            }

            delete[] tmp;

        }

        ++size;
        string[size - 1] = c;

    }

    void pop_back() {

        --size;

        if (size <= capacity / 4 &&  capacity > min_capacity) {

            char* tmp = new char[capacity / 2];
            for (size_t i = 0; i < size; ++i) {
                tmp[i] = string[i];
            }

            delete[] string;
            capacity /= 2;
            string = new char[capacity];

            for (size_t i = 0; i < size; ++i) {
                string[i] = tmp[i];
            }

            delete[] tmp;

        }

    }

    char& front() {
        return string[0];
    }

    char& back() {
        return string[size - 1];
    }

    char front() const {
        return string[0];
    }

    char back() const {
        return string[size - 1];
    }

    size_t find(const String& substring) const {

        size_t j = 0;
        size_t pos = size;

        for (size_t i = 0; i < size; ++i) {

            string[i] == substring[j] ? ++j : j = 0;

            if (j == 1) {
                pos = i;
            }

            if (j == substring.length()) {
                return pos;
            }

        }

        return size;

    }

    size_t rfind(const String& substring) const {

        size_t j = substring.length();

        for (size_t i = size; i >= 1; --i) {

            string[i - 1] == substring[j - 1] ? --j : j = substring.length();

            if (j == 0) {
                return i - 1;
            }

        }

        return size;

    }

    String substr(size_t start, size_t count) const {

        String ret;

        for (size_t i = start; i < start + count; ++i) {
            ret.push_back(string[i]);
        }

        return ret;

    }

    bool empty() const {
        return size == 0;
    }

    void clear() {

        size = 0;
        capacity = 16;
        delete[] string;
        string = new char[capacity];

    }

    String& operator+=(const String& str){

        for (size_t i = 0; i < str.length(); ++i) {
            push_back(str[i]);
        }

        return *this;

    }

private:

    static const size_t min_capacity = 16;
    size_t size = 0;
    size_t capacity = min_capacity;
    char* string;

    static size_t max(size_t a, size_t b) {
        return a > b ? a : b;
    }

};

String operator+(const String& str_1, const String& str_2) {
    String tmp(str_1);
    return tmp += str_2;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        out << str[i];
    }
    return out;
}

std::istream& operator>>(std::istream& in, String& str) {

    str = "";
    char c;

    while (in.get(c) && !in.eof()) {

        if (std::isspace(c)) {
            break;
        }

        str.push_back(c);

    }

    return in;

}

bool operator==(const String& str_1, const String& str_2) {

    if (str_1.length() != str_2.length()) {
        return false;
    } else {

        for (size_t i = 0; i < str_1.length(); ++i) {
            if (str_1[i] != str_2[i]) {
                return false;
            }
        }

    }

    return true;

}
