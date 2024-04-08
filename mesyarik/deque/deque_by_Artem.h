#include <iostream>

template<typename T>
class Deque {
public:
    Deque ();
    Deque (Deque<T> const &);
    Deque & operator =(Deque<T> const &);
    ~Deque();
    explicit Deque (int, T const & = T());
    [[nodiscard]] size_t size() const;
    T & operator [] (size_t);
    T operator [] (size_t) const;
    T& at (size_t);
    T at (size_t) const;
    void push_back(T const &);
    void pop_back();
    void push_front(T const &);
    void pop_front();
private:
    void reserve(size_t);
    void destroyBody();

    static const size_t chunk_capacity = 512;

    T** body;
    size_t cap;
    T** start;
    T** finish;

    size_t amount;
    T* first; // указатели на первый и за последний элемент дека
    T* last;
    // эти две переменные позволяют быстро обращаться к необходимым элементам в первом и последнем чанках
    // их работа основана на том, что между первым и последним чанком вся память заполнена

private:
    template <bool is_const>
    class Iterator {
    public:
        using difference_type = size_t;
        using value_type = std::conditional_t<is_const, const T, T>;
        using reference = std::conditional_t<is_const, const T&, T&>;
        using pointer = std::conditional_t<is_const, const T*, T*>;
        using iterator_category = std::random_access_iterator_tag;

        Iterator () = delete;
        Iterator (Deque<T> const &, T**, T*, size_t index);
        ~Iterator () = default;
        Iterator (Iterator const &) = default;

        Iterator & operator= (Iterator const &) = default;

        operator Iterator<true>() const;

        reference operator *() const noexcept {
            return *body;
        }

        pointer operator ->() const noexcept {
            return body;
        }

        Iterator & operator ++();
        Iterator operator ++(int);
        Iterator & operator --();
        Iterator operator --(int);

        bool operator <(Iterator const &) const;
        bool operator >(Iterator const &) const;
        bool operator <=(Iterator const &) const;
        bool operator >=(Iterator const &) const;
        bool operator ==(Iterator const &) const;
        bool operator !=(Iterator const &) const;

        Iterator & operator += (int);
        Iterator operator + (int);
        /*template <bool is_const1>
        friend Iterator<is_const1> operator +(Iterator<is_const1> const &, int);*/

        Iterator & operator -= (int);

        Iterator operator - (int);

        /*template <bool is_const1, bool is_const2>
        friend int operator -(Iterator<is_const1> const &, Iterator<is_const2> const &);*/
        template <bool is_const1>
        int operator -(Iterator<is_const1> const &);

    private:
        Deque<T> const & dq;
        T** chunk;
        T* body;
        size_t index;

        friend Deque<T>;
    };
public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    iterator begin();
    iterator end();
    const_iterator begin () const;
    const_iterator end () const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    template <bool is_const>
    void erase (Iterator<is_const>);
    template <bool is_const>
    void insert (Iterator<is_const>, T const &);
};

template<typename T>
Deque<T>::Deque() : cap(1), amount(0) {
    body = reinterpret_cast<T**>(new uint8_t*[cap * sizeof(int*)]);
    start = body;
    finish = body;
    first = *body;
    last = *body;
}

template<typename T>
Deque<T>::Deque(const Deque<T> & original) : cap(original.cap), amount(original.amount) {
    body = reinterpret_cast<T**>(new uint8_t[cap * sizeof(int*)]);
    start = body + (original.start - original.body);
    finish = body + (original.finish - original.body);
    for (T** x = original.start + 1, **xx = start + 1; x < original.finish - 1; ++x, ++xx) {
        *xx = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        for (T* y = *x, *yy = *xx; y - *x != chunk_capacity; ++y, ++yy) {
            new (yy) T(*y);
        }
    }
    first = *start + (original.first - *original.start);
    last = *(finish - 1) + (original.last - *(original.finish - 1));
    // Да, костылей много и они страшные, но что поделать, если первый и последний чанки столь особенные..
    if (start != finish) {
        if (finish - 1 == start) {
            *start = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
            first = *start + (original.first - *original.start);
            last = *(finish - 1) + (original.last - *(original.finish - 1));
            for (T* x = original.first, *xx = first; x != original.last; ++x, ++xx) {
                new (xx) T(*x);
            }
        }
        else {
            *start = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
            first = *start + (original.first - *original.start);
            for (T* x = original.first, *xx = first; x != *original.start + chunk_capacity; ++x, ++xx) {
                new (xx) T(*x);
            }
            *(finish - 1) = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
            last = *(finish - 1) + (original.last - *(original.finish - 1));
            for (T* x = *(original.finish - 1), *xx = *(finish - 1); x != original.last; ++x, ++xx) {
                new (xx) T(*x);
            }
        }
    }
}

template<typename T>
Deque<T> &Deque<T>::operator=(const Deque<T> & original) {
    if (this == &original) return *this;
    Deque<T> copy(original);
    std::swap(first, copy.first);
    std::swap(last, copy.last);
    std::swap(start, copy.start);
    std::swap(finish, copy.finish);
    std::swap(body, copy.body);
    std::swap(amount, copy.amount);
    std::swap(cap, copy.cap);
    return *this;
}

template<typename T>
Deque<T>::Deque(int amount, const T & el) : amount(amount) {
    cap = 1 << (std::__lg(amount) + 1);
    body = reinterpret_cast<T**>(new uint8_t[cap * sizeof(int*)]);
    start = body + cap / 2 - amount / chunk_capacity / 2;
    finish = start + amount / chunk_capacity;
    for (T** x = start; x != finish; ++x) {
        *x = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        for (T* y = *x; y - *x != chunk_capacity; ++y) {
            new (y) T(el);
        }
    }
    last = *(finish - 1) + chunk_capacity;
    if (amount % chunk_capacity != 0) {
        *finish = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        for (T* x = *finish; static_cast<unsigned long>(x - *finish) != amount % chunk_capacity; ++x) {
            new (x) T(el);
        }
        ++finish;
        last = *(finish - 1) + amount % chunk_capacity;
    }
    first = *start;
}

template<typename T>
Deque<T>::~Deque() {
    destroyBody();
}

template<typename T>
void Deque<T>::reserve(size_t newCap) {
    if (newCap <= cap) return;
    T** newBody = reinterpret_cast<T**>(new uint8_t[newCap * sizeof(int*)]);
    T** newStart = newBody + newCap / 2 - (finish - start) / 2 - 1;
    for (T **x = start, **y = newStart; x != finish; ++x, ++y) {
        *y = *x;
    }
    finish = newStart + (finish - start);
    start = newStart;
    delete [] reinterpret_cast<uint8_t*>(body);
    body = newBody;
    cap = newCap;
}

template<typename T>
void Deque<T>::destroyBody() {
    // finish - 1 может оказаться изначально меньше чем start + 1, потому я не могу сравнивать их через !=
    for (T** x = start + 1; x < finish - 1; ++x) {
        for (T* y = *x; y - *x != chunk_capacity; ++y) {
            y->~T();
        }
        delete [] reinterpret_cast<uint8_t*>(*x);
    }
    // Да, костылей много и они страшные, но что поделать, если первый и последний чанки столь особенные..
    if (start != finish) {
        if (finish - 1 == start) {
            for (T* x = first; x != last; ++x) {
                x->~T();
            }
            delete [] reinterpret_cast<uint8_t*>(*start);
        }
        else {
            for (T* x = first; x != *start + chunk_capacity; ++x) {
                x->~T();
            }
            delete [] reinterpret_cast<uint8_t*>(*start);
            for (T* x = *(finish - 1); x != last; ++x) {
                x->~T();
            }
            delete [] reinterpret_cast<uint8_t*>(*(finish - 1));
        }
    }
    delete [] reinterpret_cast<uint8_t*>(body);
    amount = 0;
}

template<typename T>
void Deque<T>::push_back(const T & el) {
    if (start == finish) {
        *start = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        ++finish;
        first = *start + chunk_capacity / 2;
        last = first;
    }
    if (static_cast<unsigned long>(last - *(finish - 1)) >= chunk_capacity) {
        if (static_cast<unsigned long>(finish - body) >= cap) {
            reserve(cap << 1);
        }
        *finish = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        last = *finish;
        ++finish;
    }
    new (last++) T(el);
    ++amount;
}

template<typename T>
void Deque<T>::pop_back() {
    --amount;
    (--last)->~T();
    // если последний чанк пустой
    if (last == *(finish - 1)) {
        --finish;
        delete [] reinterpret_cast<uint8_t*>(*finish);
        last = *(finish - 1) + chunk_capacity;
    }
}

template<typename T>
void Deque<T>::push_front(const T & el) {
    if (start == finish) {
        *start = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        ++finish;
        first = *start + chunk_capacity / 2;
        last = first;
    }
    if (first == *start) {
        if (start == body) {
            reserve(cap << 1);
        }
        if (start == body) {
            reserve(cap << 1); // костыль, чтобы при увеличении cap с 1 до 2 правильно создавалась память
        }
        --start;
        *start = reinterpret_cast<T*>(new uint8_t[chunk_capacity * sizeof(T)]);
        first = *start + chunk_capacity;
    }
    new (--first) T(el);
    ++amount;
}

template<typename T>
void Deque<T>::pop_front() {
    --amount;
    (first++)->~T();
    // если первый чанк пустой
    if (first == *start + chunk_capacity) {
        delete [] reinterpret_cast<uint8_t*>(*start);
        ++start;
        first = *start;
    }
}

template<typename T>
T &Deque<T>::operator[](size_t index) {
    size_t firstSz = (*start + chunk_capacity) - first;
    if (index < firstSz) return *(first + index);
    index -= firstSz;
    return *(*(start + index / chunk_capacity + 1) + index % chunk_capacity);
}

template<typename T>
T Deque<T>::operator[](size_t index) const {
    size_t firstSz = (*start + chunk_capacity) - first;
    if (index < firstSz) return T{*(first + index)};
    index -= firstSz;
    return T{*(*(start + index / chunk_capacity + 1) + index % chunk_capacity)};
}

template<typename T>
T &Deque<T>::at(size_t index) {
    if (index >= amount) throw std::out_of_range("#%!%&!");
    return (*this)[index];
}

template<typename T>
T Deque<T>::at(size_t index) const {
    if (index >= amount) throw std::out_of_range("#%!%&!");
    return (*this)[index];
}

template<typename T>
size_t Deque<T>::size() const {
    return amount;
}

template<typename T>
typename Deque<T>::template Iterator<false> Deque<T>::begin() {
    if (start == finish) return Iterator<false>(*this, start, nullptr, 0);
    return Iterator<false>(*this, start, first, 0);
}

template<typename T>
typename Deque<T>::template Iterator<true> Deque<T>::begin() const {
    if (start == finish) return Iterator<true>(*this, start, nullptr, 0);
    return Iterator<true>(*this, start, first, 0);
}

template<typename T>
typename Deque<T>::template Iterator<false> Deque<T>::end() {
    if (finish == start) return Iterator<false>(*this, finish, nullptr, 0);
    return Iterator<false>(*this, finish - 1, last, amount);
}

template<typename T>
typename Deque<T>::template Iterator<true> Deque<T>::end() const {
    if (start == finish) return Iterator<true>(*this, finish, nullptr, 0);
    return Iterator<true>(*this, finish - 1, last, amount);
}

template<typename T>
typename Deque<T>::template Iterator<true> Deque<T>::cbegin() const {
    if (start == finish) return Iterator<true>(*this, start, nullptr, 0);
    return Iterator<true>(*this, start, first, 0);
}

template<typename T>
typename Deque<T>::template Iterator<true> Deque<T>::cend() const {
    if (finish == start) return Iterator<true>(*this, finish, nullptr, 0);
    return Iterator<true>(*this, finish - 1, last, amount);
}

template<typename T>
template<bool is_const>
void Deque<T>::erase(Deque::Iterator<is_const> it) {
    for (size_t i = it.index; i < amount - 1; ++i) {
        std::swap((*this)[i], (*this)[i + 1]);
    }
    pop_back();
}

template<typename T>
template<bool is_const>
void Deque<T>::insert(Deque::Iterator<is_const> it, const T & el) {
    push_back(el);
    for (size_t i = amount - 1; i > it.index; --i) {
        std::swap((*this)[i], (*this)[i - 1]);
    }
}

template<typename T>
template<bool is_const>
Deque<T>::Iterator<is_const>::Iterator(Deque<T> const & dq, T **chunk, T *body, size_t index):
        dq(dq), chunk(chunk), body(body), index(index) {}


template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> &Deque<T>::Iterator<is_const>::operator++() {
    ++body;
    ++index;
    if (body - *chunk == chunk_capacity) {
        if (dq.finish == chunk + 1) return *this;
        ++chunk;
        body = *chunk;
    }
    return *this;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> Deque<T>::Iterator<is_const>::operator++(int) {
    auto cpy = *this;
    ++(*this);
    return cpy;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> Deque<T>::Iterator<is_const>::operator--(int) {
    auto cpy = *this;
    --(*this);
    return cpy;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> &Deque<T>::Iterator<is_const>::operator--() {
    if (body - *chunk == 0) {
        if (dq.start == chunk) return *this;
        --chunk;
        body = *chunk + chunk_capacity - 1;
    }
    else {
        --body;
    }
    --index;
    return *this;
}

template<typename T>
template<bool is_const>
bool Deque<T>::Iterator<is_const>::operator==(const Deque<T>::Iterator<is_const> & right) const {
    return body == right.body;
}

template<typename T>
template<bool is_const>
bool Deque<T>::Iterator<is_const>::operator!=(const Deque::Iterator<is_const> & right) const {
    return !(*this == right);
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> &Deque<T>::Iterator<is_const>::operator+=(int delta) {
    index += delta;
    size_t idx = body - *chunk;
    chunk += delta / chunk_capacity;
    if (idx + delta % chunk_capacity > chunk_capacity) {
        ++chunk;
    }
    body = *chunk + (idx + delta) % chunk_capacity;

    /*if (&body - chunk + delta % chunk_capacity > chunk_capacity) {
        T** ch = chunk;
        chunk += delta / chunk_capacity + 1;
        body = *chunk + (&body - ch + delta - 1) % chunk_capacity;
    }
    else {
        T** ch = chunk;
        chunk += delta / chunk_capacity;
        body = *chunk + (&body - ch + delta) % chunk_capacity;
    }*/
    return *this;
}

template<typename T>
template<bool is_const>
Deque<T>::Iterator<is_const>::operator Iterator<true>() const {
    const_iterator it{dq, chunk, body, index};
    return it;
}

/*template<typename T>
//template<typename T, bool is_const>
typename Deque<T>::iterator operator+
        (typename Deque<T>::iterator const & it, int delta) {
    auto cpy = it;
    cpy += delta;
    return cpy;
}*/

template<typename T>
template<bool is_const>
bool Deque<T>::Iterator<is_const>::operator<(const Deque::Iterator<is_const> & right) const {
    return index < right.index;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> Deque<T>::Iterator<is_const>::operator+(int delta) {
    Iterator<is_const> cpy = *this;
    cpy += delta;
    return cpy;
}

template<typename T>
template<bool is_const>
bool Deque<T>::Iterator<is_const>::operator>(const Deque::Iterator<is_const> & right) const {
    return right < *this;
}

template<typename T>
template<bool is_const>
bool Deque<T>::Iterator<is_const>::operator<=(const Deque::Iterator<is_const> & right) const {
    return *this < right || right == *this;
}

template<typename T>
template<bool is_const>
bool Deque<T>::Iterator<is_const>::operator>=(const Deque::Iterator<is_const> & right) const {
    return right < *this || *this == right;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> &Deque<T>::Iterator<is_const>::operator-=(int delta) {
    /*index += delta;
    size_t idx = body - *chunk;
    chunk += delta / chunk_capacity;
    if (idx + delta % chunk_capacity > chunk_capacity) {
        ++chunk;
    }
    body = *chunk + (idx + delta) % chunk_capacity;*/
    index -= delta;
    size_t idx = body - *chunk;
    chunk -= delta / chunk_capacity;
    if (chunk_capacity + idx - delta % chunk_capacity < chunk_capacity) {
        --chunk;
    }
    body = *chunk + (chunk_capacity + idx - delta) % chunk_capacity;

    /*if (static_cast<unsigned long>(&body - chunk) < delta % chunk_capacity) {
        T** ch = chunk;
        chunk -= delta / chunk_capacity;
        --chunk;
        body = *chunk + (body - *ch - delta) % chunk_capacity;
    }
    else {
        T** ch = chunk;
        chunk -= delta / chunk_capacity;
        body = *chunk + (body - *ch + delta) % chunk_capacity;
    }*/
    return *this;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template Iterator<is_const> Deque<T>::Iterator<is_const>::operator-(int delta) {
    auto cpy = *this;
    cpy -= delta;
    return cpy;
}

template<typename T>
template<bool is_const>
template<bool is_const1>
int Deque<T>::Iterator<is_const>::operator-(const Deque::Iterator<is_const1> & right) {
    return static_cast<difference_type>(index - right.index);
}

template<typename T, bool is_const>
int operator-(const typename Deque<T>::template Iterator<is_const> & left,
        const typename Deque<T>::template Iterator<is_const> & right) {
    return static_cast<int>(left.index - right.index);
}

/*
template <typename T, bool is_const1, bool is_const2>
int Deque<T>::Iterator<is_const1>::operator -(typename Deque<T>::template Iterator<is_const2> const &) {

}*/
