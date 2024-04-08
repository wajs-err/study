#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H


#include <exception>
#include <cstddef>
#include <memory>


// N is the size of StackStorage in bytes
template <size_t N>
class StackStorage {

public:

    StackStorage(): storage(), current(static_cast<void*>(&storage)), remaining_size(N) {}
    StackStorage(const StackStorage&) = delete;
    StackStorage& operator=(const StackStorage&) = delete;
    StackStorage(StackStorage&&) = delete;
    StackStorage& operator=(StackStorage&&) = delete;
    ~StackStorage() = default;

    template <typename T>
    T* allocate(size_t n) {

        if (std::align(alignof(T), sizeof(T), current, remaining_size) != nullptr) {
            T* pointer = static_cast<T*>(current);
            current = static_cast<void*>(static_cast<std::byte*>(current) + n * sizeof(T));
            remaining_size -= n * sizeof(T);
            return pointer;
        }

        throw std::bad_alloc();

    }

private:

    std::byte storage[N];
    void* current;
    size_t remaining_size;

};


// N is the size of used StackStorage in bytes
template <typename T, size_t N>
class StackAllocator {

public:

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    StackAllocator() = delete;
    StackAllocator(const StackAllocator&) noexcept = default;
    StackAllocator& operator=(const StackAllocator&) noexcept = default;
    StackAllocator(StackAllocator&& allocator) noexcept = default;
    StackAllocator& operator=(StackAllocator&& allocator) = delete;
    ~StackAllocator() = default;

    explicit StackAllocator(StackStorage<N>& storage): storage(&storage) {}

    template <typename U>
    StackAllocator(const StackAllocator<U, N>& allocator): storage(allocator.storage) {}

    pointer allocate(size_type n) {
        return storage->template allocate<value_type>(n);
    }

    void deallocate(pointer, size_type) noexcept {}

    // max_size for StackAllocator<T, N> is N / size of the object representation of T with alignment in bytes
    // one item of T requires alignof(T) * ceil(sizeof(T) / alignof(T))
    size_type max_size() const noexcept {
        return N / ((sizeof(value_type) + alignof(value_type) - 1) / alignof(value_type) * alignof(value_type));
    }

    friend bool operator==(const StackAllocator& lhs, const StackAllocator& rhs) noexcept {
        return lhs.storage == rhs.storage;
    }

    friend bool operator!=(const StackAllocator& lhs, const StackAllocator& rhs) noexcept {
        return lhs.storage == rhs.storage;
    }

private:

    template <typename U, size_t M>
    friend class StackAllocator;

    StackStorage<N>* storage;

};


#endif // STACK_ALLOCATOR_H