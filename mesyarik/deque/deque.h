// This code is very very bad!

// There is no any warranty about this code.
// No warranty that any of functions work or even code compiles.
// It was made only for passing mesyarik's tests because I lost any hope for writing good deque :(


#ifndef _DEQUE_H_
#define _DEQUE_H_

#include <iostream>
#include <exception>


namespace deque {

    template <typename T>
    class BaseDeque {

        using removed_pointer_T = std::remove_pointer_t<T>;

    public:

        BaseDeque(): array(nullptr), size_(0), capacity_(0), borders({0, 0}) {}
        BaseDeque(size_t count, const T& value)
                : array(new T[count]), size_(count), capacity_(count), borders({0, count}) {
            for (size_t i = 0; i < count; ++i) {
                array[i] = value;
            }
        }
        BaseDeque(const BaseDeque& other) = delete;
        BaseDeque& operator=(const BaseDeque&) = delete;
        ~BaseDeque() {
            delete[] array;
        }

        T& operator[](size_t index) noexcept {
            return array[borders.begin + index];
        }

        T operator[](size_t index) const noexcept {
            return array[borders.begin + index];
        }

        size_t size() const noexcept {
            return size_;
        }

        void push_back(const T& value) {
            if (borders.end == capacity_) {
                reallocate_end();
            }
            array[borders.end] = value;
            ++borders.end;
            ++size_;
        }

        void push_front(const T& value) {
            if (borders.begin == 0) {
                reallocate_begin();
            }
            if (borders.begin != 0) {
                --borders.begin;
            } else {
                ++borders.end;
            }
            array[borders.begin] = value;
            ++size_;
        }

        void pop_back() {
            --borders.end;
            --size_;
        }

        void pop_front() {
            ++borders.begin;
            --size_;
        }

        bool empty() const noexcept {
            return size_ == 0;
        }

        T& front() const noexcept {
            return array[borders.begin];
        }
        T& back() const noexcept {
            return array[borders.end - 1];
        }

        T* begin() const noexcept {
            return array + borders.begin;
        }
        T* end() const noexcept {
            return array + borders.end;
        }

    private:

        struct Borders {
            size_t begin;
            size_t end;
        };

        T* array;
        size_t size_;
        size_t capacity_;
        Borders borders; // [begin, end) - borders of memory with constructed objects

        void reallocate_begin() {

            capacity_ += (size_ == 0 ? 1 : size_);
            T* new_array = new T[capacity_];

            for (size_t i = 0; i < size_; ++i) {
                new_array[size_ + i] = array[i];
            }

            delete[] array;
            array = new_array;

            borders.begin = size_;
            borders.end += size_;

        }

        void reallocate_end() {

            capacity_ += (size_ == 0 ? 1 : size_);
            T *new_array = new T[capacity_];

            for (size_t i = 0; i < size_; ++i) {
                new_array[borders.begin + i] = array[borders.begin + i];
            }

            delete[] array;
            array = new_array;

        }

        void destroy_copy(size_t count) noexcept {
            for (size_t j = 0; j < count; ++j) {
                delete array[j];
            }
            delete[] array;
            size_ = 0;
            capacity_ = 0;
            borders = {0, 0};
        }

        friend std::ostream& operator<<(std::ostream& out, const BaseDeque& deque) {
            if (deque.empty()) {
                out << "_ ";
            }
            for (size_t i = 0; i < deque.size_; ++i) {
                out << deque[i] << ' ';
            }
            return out;
        }

    };

    template <typename T, size_t Size>
    class Array {

    public:

        Array(): array(static_cast<T*>(::operator new(Size * sizeof(T)))) {}
        ~Array() {
            ::operator delete(array);
        }

        T& operator[](size_t index) noexcept {
            return array[index];
        }
        T operator[](size_t index) const noexcept {
            return array[index];
        }

        void push(size_t index, const T& value) {
            new(array + index) T{value};
        }
        void pop(size_t index) {
            (array + index)->~T();
        }

        T* begin() const noexcept {
            return array;
        }
        T* end() const noexcept {
            return array + Size;
        }

    private:

        T* array;

    };

    template <typename T>
    class Deque {

        friend std::ostream& operator<<(std::ostream& out, const Deque& deque) {
            if (deque.empty()) {
                out << "_ ";
            }
            for (size_t i = 0; i < deque.size_; ++i) {
                out << deque[i] << ' ';
            }
            return out;
        }

    public:

        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;

    private:

        static const size_t chunk_size = 512;
        using chunk = Array<T, chunk_size>;

        BaseDeque<chunk*> external;
        size_type size_;
        size_type first;

        template <bool IsConst>
        class Iterator;

        void initialize(int count, const T& value) {
            for (size_type i = 0; i < external.size(); ++i) {
                for (size_type j = 0; j < chunk_size && i * chunk_size + j < static_cast<size_type>(count); ++j) {
                    (*external[i])[j] = value;
                }
            }
        }

        void destroy() noexcept {
            while (size_ != 0) {
                pop_back();
            }
        }

        void check_range(size_type index) {
            if (index >= size_) {
                throw std::out_of_range("Out of range: the given index is " + std::to_string(index) +
                                        ", while the size of the deque is " + std::to_string(size_));
            }
        }

    public:

        using iterator = Iterator<false>;
        using const_iterator = Iterator<true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Deque(): external(), size_(0), first(0) {}
        explicit Deque(size_type count): Deque(count, T{}) {}
        Deque(size_type count, const T& value)
                : external((count + chunk_size - 1) / chunk_size, new chunk),
                  size_(static_cast<size_type>(count)),
                  first(0) {

            initialize(count, value);

        }

        Deque(const Deque& other): Deque() {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(other[i]);
            }
        }
        Deque& operator=(const Deque& other) {
            while (size_ > 0) {
                pop_back();
            }
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(other[i]);
            }
            return *this;
        }
        Deque(Deque&&) noexcept = default;
        Deque& operator=(Deque&&) noexcept = default;
        ~Deque() {
            destroy();
        }

        size_t size() const noexcept {
            return size_;
        }

        bool empty() const noexcept {
            return size_ == 0;
        }

        void push_front(const value_type& value) {
            if (first == 0) {
                external.push_front(new chunk);
            }
            (first += chunk_size - 1) %= chunk_size;
            external[0]->push(first, value);
            ++size_;
        }

        void push_back(const value_type& value) {
            if ((first + size_) % chunk_size == 0) {
                external.push_back(new chunk);
            }
            external[(first + size_) / chunk_size]->push((first + size_) % chunk_size, value);
            ++size_;
        }

        void pop_front() noexcept {
            external[0]->pop(first);
            (++first) %= chunk_size;
            if (first == 0 || size_ == 1) {
                delete external[0];
                external.pop_front();
            }
            --size_;
            if (size_ == 0) {
                first = 0;
            }
        }

        void pop_back() noexcept {
            external.back()->pop((first + size_ - 1) % chunk_size);
            if ((first + size_ - 1) % chunk_size == 0 && size_ != 1) {
                delete external.back();
                external.pop_back();
            }
            --size_;
            if (size_ == 0) {
                delete external[0];
                external.pop_back();
                first = 0;
            }
        }

        void insert(iterator it, const value_type& value) {
            push_back(value);
            size_type pos = it - begin();
            for (ssize_t i = size_ - 1; i > static_cast<ssize_t>(pos); --i) {
                std::swap((*this)[i], (*this)[i - 1]);
            }
        }

        void erase(iterator it) {
            size_type pos = it - begin();
            for (size_type i = pos; i < size_ - 1; ++i) {
                std::swap((*this)[i], (*this)[i + 1]);
            }
            pop_back();
        }

        reference operator[](size_type index) noexcept {
            return (*external[(first + index) / chunk_size])[(first + index) % chunk_size];
        }
        const_reference operator[](size_type index) const noexcept {
            return (*external[(first + index) / chunk_size])[(first + index) % chunk_size];
        }

        reference at(size_type index) {
            check_range(index);
            return (*this)[index];
        }
        const_reference at(size_type index) const {
            check_range(index);
            return (*this)[index];
        }

        iterator begin() noexcept {
            return get_begin();
        }
        const_iterator cbegin() const noexcept {
            return get_begin();
        }
        const_iterator begin() const noexcept {
            return cbegin();
        }

        reverse_iterator rbegin() noexcept {
            return get_rbegin();
        }
        const_reverse_iterator crbegin() const noexcept {
            return get_rbegin();
        }
        const_reverse_iterator rbegin() const noexcept {
            return crbegin();
        }

        iterator end() noexcept {
            return get_end();
        }
        const_iterator cend() const noexcept {
            return get_end();
        }
        const_iterator end() const noexcept {
            return cend();
        }

        reverse_iterator rend() noexcept {
            return get_rend();
        }
        const_reverse_iterator crend() const noexcept {
            return get_rend();
        }
        const_reverse_iterator rend() const noexcept {
            return crend();
        }

    private:

        iterator get_begin() const noexcept {
            if (empty()) {
                return {external.begin(), nullptr};
            }
            return {external.begin(), &(*external[0])[first]};
        }

        iterator get_end() const noexcept {
            if (empty()) {
                return {external.begin(), nullptr};
            }
            size_t f = (first + size_) % chunk_size == 0 ? chunk_size : 0;
            return {external.end() - 1,
                    &(*external.back())[(first + size_) % chunk_size] + f};
        }

        reverse_iterator get_rbegin() const noexcept {
            return std::reverse_iterator(end());
        }

        reverse_iterator get_rend() const noexcept {
            return std::reverse_iterator(begin());
        }

    };

    template <typename T>
    template <bool IsConst>
    class Deque<T>::Iterator {

    public:

        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator_category = std::random_access_iterator_tag;

        Iterator(): external_index(), internal_index() {}
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;
        ~Iterator() = default;

        std::conditional_t<IsConst, const_reference, reference> operator*() const noexcept {
            return *internal_index;
        }

        std::conditional_t<IsConst, const_pointer, pointer> operator->() const noexcept {
            return internal_index;
        }

        Iterator& operator++() {
            ++internal_index;
            if (internal_index == (*external_index)->end() && *(external_index + 1) != nullptr) {
                ++external_index;
                internal_index = (*external_index)->begin();
            }
            return *this;
        }

        Iterator& operator--() {
            if (internal_index == (*external_index)->begin()) {
                --external_index;
                internal_index = (*external_index)->end() - 1;
            } else {
                --internal_index;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator it(*this);
            ++*this;
            return it;
        }

        Iterator operator--(int) {
            Iterator it(*this);
            --*this;
            return it;
        }

        difference_type operator-(const Iterator& rhs) const noexcept {
            if (internal_index == nullptr && rhs.internal_index == nullptr) {
                return 0;
            }
            return (external_index - rhs.external_index) * chunk_size
                   + (internal_index - (*external_index)->begin() + 1)
                   - (rhs.internal_index - (*rhs.external_index)->begin() + 1);
        }

        bool operator==(const Iterator& rhs) const noexcept {
            return external_index == rhs.external_index && internal_index == rhs.internal_index;
        }

        bool operator!=(const Iterator& rhs) const noexcept {
            return !(*this == rhs);
        }

        bool operator<(const Iterator& rhs) const noexcept {
            return external_index < rhs.external_index ||
                   (external_index == rhs.external_index && internal_index < rhs.internal_index);
        }

        bool operator>(const Iterator& rhs) const noexcept {
            return rhs < *this;
        }

        bool operator<=(const Iterator& rhs) const noexcept {
            return !(rhs < *this);
        }

        bool operator>=(const Iterator& rhs) const noexcept {
            return !(*this < rhs);
        }

        operator Iterator<true>() const {
            return {external_index, internal_index};
        }

        Iterator operator+(difference_type n) const {
            Iterator it = *this;
            for (size_t i = 0; i < static_cast<size_t>(n); ++i) {
                ++it;
            }
            return it;
        }

        Iterator operator-(difference_type n) const {
            Iterator it = *this;
            for (size_t i = 0; i < static_cast<size_t>(n); ++i) {
                --it;
            }
            return it;
        }

        friend Iterator operator+(difference_type n, const Iterator& iterator) {
            return iterator + n;
        }

    private:

        friend Deque<value_type>;

        chunk** external_index;
        pointer internal_index;

        Iterator(chunk** external_index, value_type* internal_index)
                : external_index(external_index), internal_index(internal_index) {}

    };

}
using deque::Deque;

#endif // _DEQUE_H_