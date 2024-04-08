//
// Author: Maximov Daniil
//

#ifndef HWSTRING_STRING_HPP
#define HWSTRING_STRING_HPP 1

#include <iostream>
#include <cstring>

class String {
private:
    size_t m_size;
    size_t m_capacity;
    char* m_str;

    void _update_str();
    void _update_capacity(const size_t size);
    void _fit(const size_t size);
public:
    String();
    String(const char* c_str);
    String(char c);
    String(size_t n, char c);
    String(const String& obj);

    ~String();

    size_t length() const;
    bool empty() const;
    char& front();
    char front() const;
    char& back();
    char back() const;

    void push_back(char c);
    void pop_back();
    void clear();

    char& operator[](size_t index);
    char operator[](size_t index) const;
    String& operator=(const String& obj);
    String& operator+=(const String& obj);
    bool operator==(const String& obj) const;

    size_t find(const String& str) const;
    size_t rfind(const String& str) const;

    String substr(size_t start, size_t count) const;

    friend std::istream& operator>>(std::istream& in, String& str);
    friend std::ostream& operator<<(std::ostream& out, const String& str);
};

void String::_update_capacity(const size_t size) {
    while (size > m_capacity) {
        m_capacity <<= 1;
    }
}

void String::_update_str() {
    delete[] m_str;
    m_str = new char[m_capacity];
}

void String::_fit(const size_t size) {
    _update_capacity(size);
    _update_str();
}

String::String(): m_size(0), m_capacity(1), m_str(new char[1]) {}

String::String(const char* str): String() {
    m_size = strlen(str);

    _fit(m_size);

    memcpy(m_str, str, m_size);
}

[[maybe_unused]] String::String(char c): String() {
    m_size = 1;
    m_str[0] = c;
}

String::String(size_t n, char c): String() {
    m_size = n;

    _fit(m_size);

    memset(m_str, c, m_size);
}

String::String(const String& obj): m_size(obj.m_size),
                                   m_capacity(obj.m_capacity),
                                   m_str(new char[obj.m_capacity]) {
    memcpy(m_str, obj.m_str, m_size);
}

String::~String() {
    delete[] m_str;
}

size_t String::length() const {
    return m_size;
}

bool String::empty() const {
    return m_size == 0;
}

char& String::front() {
    return m_str[0];
}

char String::front() const {
    return m_str[0];
}

char& String::back() {
    return m_str[m_size - 1];
}

char String::back() const {
    return m_str[m_size - 1];
}

void String::push_back(char c) {
    if (m_size == m_capacity) {
        String copy = *this;

        _fit(m_size + 1);

        memcpy(m_str, copy.m_str, m_size);
    }

    m_str[m_size] = c;
    ++m_size;
}

 void String::pop_back() {
    --m_size;
}

void String::clear() {
    m_size = 0;
}

char& String::operator[](size_t index) {
    return m_str[index];
}

char String::operator[](size_t index) const {
    return m_str[index];
}

String& String::operator=(const String& obj) {
    String copy = obj;

    m_size = copy.m_size;
    m_capacity = copy.m_capacity;
    std::swap(m_str, copy.m_str);

    return *this;
}

String& String::operator+=(const String& obj) {
    if (m_size + obj.m_size <= m_capacity) {
        memcpy(m_str + m_size, obj.m_str, obj.m_size);
    } else {
        size_t newSize = m_size + obj.m_size;

        _update_capacity(newSize);

        char* buffer = new char[m_capacity];

        memcpy(buffer, m_str, m_size);
        memcpy(buffer + m_size, obj.m_str, obj.m_size);

        std::swap(m_str, buffer);

        delete[] buffer;
    }

    m_size += obj.m_size;

    return *this;
}

String operator+(const String& obj1, const String& obj2) {
    String copy = obj1;
    copy += obj2;
    return copy;
}

bool String::operator==(const String& obj) const {
    if (m_size != obj.m_size) return false;

    for (size_t i = 0; i < m_size; ++i) {
        if (m_str[i] != obj[i]) return false;
    }

    return true;
}

size_t String::find(const String &str) const {
    if (str.m_size > m_size) return m_size;

    const char* end = m_str + (m_size + 1 - str.m_size);
    bool isEqual;
    size_t k;

    for (char* p = m_str; p != end; ++p) {
        isEqual = true;
        k = 0;

        while (k < str.m_size && isEqual) {
            isEqual = str[k] == *(p + k);
            ++k;
        }

        if (isEqual) return p - m_str;
    }

    return m_size;
}

size_t String::rfind(const String &str) const {
    if (str.m_size > m_size) return m_size;

    const char* end = m_str - 1;
    bool isEqual;
    size_t k;

    for (char* p = m_str + (m_size - str.m_size); p != end; --p) {
        isEqual = true;
        k = 0;

        while (k < str.m_size && isEqual) {
            isEqual = str[k] == *(p + k);
            ++k;
        }

        if (isEqual) return p - m_str;
    }

    return m_size;
}

String String::substr(size_t start, size_t count) const {
    count = std::min(count, m_size - start);

    String str;
    str.m_size = count;
    str._fit(count);

    size_t limit = start + count;

    for (size_t i = start; i < limit; ++i) {
        str[i - start] = m_str[i];
    }

    return str;
}

std::istream& operator>>(std::istream& in, String& str) {
    char c = ' ';

    while (!in.eof() && std::isspace(c)) {
        in.get(c);
    }

    if (!std::isspace(c)) {
        str = c;

        while (!in.eof() && !std::isspace(in.peek())) {
            in.get(c);
            str += c;
        }
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
    for (size_t i = 0; i < str.m_size; ++i) {
        out << str[i];
    }

    return out;
}

#endif //HWSTRING_STRING_HPP
