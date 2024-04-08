#ifndef SHARED_PTR_H
#define SHARED_PTR_H


#include <memory>


template <typename T>
class WeakPtr;


namespace detail {


    /*
     * SharedPtr(C-style pointer) -> custom Allocator and Deleter, using ControlBLock
     * allocateShared(allocator, args...) -> custom Allocator only, using ControlBlockInplace
     * makeShared(args...) -> no custom Allocator or Deleter, using ControlBlockInplace
     */


    // superclass for different control blocks
    struct BaseControlBlock {

        size_t shared_count;    // number of SharedPtrs
        size_t weak_count;      // number of WeakPtrs

        // this function is called when shared_count drops to zero
        virtual void dispose() noexcept = 0;

        // this function is called when weak_count and shared_count both drop to zero
        virtual void destroy() noexcept = 0;

        virtual ~BaseControlBlock() = default;

        BaseControlBlock(size_t shared_count, size_t weak_count)
                : shared_count(shared_count), weak_count(weak_count) {}

    };

    // SharedPtr created by SharedPtr(Y*)
    template <typename T, typename Deleter, typename Allocator>
    struct ControlBlockPtr final : BaseControlBlock {

        Deleter deleter;
        Allocator allocator;
        T* object;

        void destroy() noexcept final {
            using ControlBlockAllocator
                    = typename std::allocator_traits<Allocator>::template rebind_alloc<ControlBlockPtr>;
            ControlBlockAllocator alloc;
            // why not std::allocator_traits<ControlBlockAllocator>::destroy(alloc, this) ?
            this->~ControlBlockPtr();
            std::allocator_traits<ControlBlockAllocator>::deallocate(alloc, this, 1);
        }

        void dispose() noexcept final {
            deleter(object);
        }

        template <typename... Args>
        ControlBlockPtr(size_t shared_count, size_t weak_count, Deleter deleter, Allocator allocator, Args&&... args)
                : BaseControlBlock(shared_count, weak_count), deleter(deleter), allocator(allocator) {
            object = std::allocator_traits<Allocator>::allocate(allocator, 1);
            std::allocator_traits<Allocator>::construct(allocator, object, std::forward<Args>(args)...);
        }

        ControlBlockPtr(size_t shared_count, size_t weak_count, Deleter deleter, Allocator allocator, T* ptr)
                : BaseControlBlock(shared_count, weak_count), deleter(deleter), allocator(allocator), object(ptr) {}

        ~ControlBlockPtr() final = default;

    };

    // SharedPtr created by makeShared() or allocateShared()
    template <typename T, typename Allocator>
    struct ControlBlockInplace final : BaseControlBlock {

        Allocator allocator;
        T object;

        void destroy() noexcept final {
            using ControlBlockAllocator
                    = typename std::allocator_traits<Allocator>::template rebind_alloc<ControlBlockInplace>;
            ControlBlockAllocator alloc;
            allocator.~Allocator();
            std::allocator_traits<ControlBlockAllocator>::deallocate(alloc, this, 1);
        }

        void dispose() noexcept final {
            std::allocator_traits<Allocator>::destroy(allocator, &object);
        }

        template <typename... Args>
        ControlBlockInplace(size_t shared_count, size_t weak_count, Allocator allocator, Args&&... args)
                : BaseControlBlock(shared_count, weak_count), allocator(allocator),
                  object(std::forward<Args>(args)...) {}

        ~ControlBlockInplace() final = default;

    };

}


template <typename T>
class SharedPtr {

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

    template <typename U, typename Allocator, typename... Args>
    friend SharedPtr<U> allocateShared(const Allocator&, Args&&...);

private:

    template <typename U, typename Y>
    using is_compatible_t = std::enable_if_t<std::is_convertible_v<U*, Y*>>;

    detail::BaseControlBlock* control_block;
    T* object;

    explicit SharedPtr(detail::BaseControlBlock* control_block, T* object)
            : control_block(control_block), object(object) {
        increase_shared_count();
    }

    template <typename U, typename Deleter, typename Allocator>
    explicit SharedPtr(detail::ControlBlockPtr<U, Deleter, Allocator>* control_block)
            : control_block(control_block), object(control_block->object) {}

    template <typename Allocator>
    explicit SharedPtr(detail::ControlBlockInplace<T, Allocator>* control_block)
            : control_block(control_block), object(&control_block->object) {}

    SharedPtr& operator=(std::nullptr_t null_ptr) {
        control_block = null_ptr;
        object = null_ptr;
        return *this;
    }

    void increase_shared_count() noexcept {
        if (control_block != nullptr) {
            ++control_block->shared_count;
        }
    }

public:

    using element_type = T;
    using weak_type = WeakPtr<T>;

    SharedPtr() noexcept : control_block(nullptr), object(nullptr) {}

    SharedPtr(const SharedPtr& other) noexcept : control_block(other.control_block), object(other.object) {
        increase_shared_count();
    }

    template <typename Y, typename = is_compatible_t<Y, T>>
    SharedPtr(const SharedPtr<Y>& other) noexcept : control_block(other.control_block), object(other.object) {
        increase_shared_count();
    }

    SharedPtr(SharedPtr&& other) noexcept : control_block(other.control_block), object(other.object) {
        other = nullptr;
    }

    template <typename Y, typename = is_compatible_t<Y, T>>
    SharedPtr(SharedPtr<Y>&& other) noexcept : control_block(other.control_block), object(other.object) {
        other = nullptr;
    }

    template <typename Y, typename = is_compatible_t<Y, T>>
    explicit SharedPtr(Y* ptr) : SharedPtr(ptr, std::default_delete<Y>(), std::allocator<Y>()) {}

    template <typename Y, typename Deleter, typename = is_compatible_t<Y, T>>
    SharedPtr(Y* ptr, Deleter deleter) : SharedPtr(ptr, deleter, std::allocator<Y>()) {}

    template <typename Y, typename Deleter, typename Allocator, typename = is_compatible_t<Y, T>>
    SharedPtr(Y* ptr, Deleter deleter, Allocator allocator) {

        using ControlBlock = detail::ControlBlockPtr<Y, Deleter, Allocator>;
        using ControlBlockAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<ControlBlock>;

        ControlBlockAllocator alloc;
        auto p = std::allocator_traits<ControlBlockAllocator>::allocate(alloc, 1);
        ::new(p) detail::ControlBlockPtr<Y, Deleter, Allocator>(1, 0, deleter, allocator, ptr);

        control_block = static_cast<detail::BaseControlBlock*>(p);
        object = static_cast<detail::ControlBlockPtr<Y, Deleter, Allocator>*>(control_block)->object;

    }

    SharedPtr& operator=(const SharedPtr& other) {
        SharedPtr(other).swap(*this);
        return *this;
    }

    template <typename Y, typename = is_compatible_t<Y, T>>
    SharedPtr& operator=(const SharedPtr<Y>& other) {
        SharedPtr(other).swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(std::move(other)).swap(*this);
        return *this;
    }

    template <typename Y, typename = is_compatible_t<Y, T>>
    SharedPtr& operator=(SharedPtr<Y>&& other) noexcept {
        SharedPtr(std::move(other)).swap(*this);
        return *this;
    }

    ~SharedPtr() {

        if (control_block == nullptr) {
            return;
        }

        --control_block->shared_count;

        if (control_block->shared_count == 0) {
            control_block->dispose();
        }

        if (control_block->shared_count == 0 && control_block->weak_count == 0) {
            control_block->destroy();
        }

    }

    void reset() noexcept {
        SharedPtr().swap(*this);
    }

    template <typename Y>
    void reset(Y* ptr) {
        SharedPtr(ptr).swap(*this);
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(control_block, other.control_block);
        std::swap(object, other.object);
    }

    T* get() const noexcept {
        return object;
    }

    T& operator*() const noexcept {
        return *get();
    }

    T* operator->() const noexcept {
        return get();
    }

    size_t use_count() const noexcept {
        if (control_block == nullptr) {
            return 0;
        }
        return control_block->shared_count;
    }

};


template <typename T, typename Allocator, typename... Args>
SharedPtr<T> allocateShared(const Allocator& allocator, Args&&... args) {

    using ControlBLock = detail::ControlBlockInplace<T, Allocator>;
    using ControlBlockAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<ControlBLock>;

    ControlBlockAllocator cb_alloc;
    auto ptr = std::allocator_traits<ControlBlockAllocator>::allocate(cb_alloc, 1);
    std::allocator_traits<ControlBlockAllocator>::construct(
            cb_alloc, ptr, 1, 0, allocator, std::forward<Args>(args)...);

    return SharedPtr<T>(ptr);

}


template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}


template <typename T>
class WeakPtr {

    template <typename U>
    friend class WeakPtr;

private:

    detail::BaseControlBlock* control_block;
    T* object;

    WeakPtr& operator=(std::nullptr_t null_ptr) {
        control_block = null_ptr;
        object = null_ptr;
        return *this;
    }

    void increase_weak_count() noexcept {
        if (control_block != nullptr) {
            ++control_block->weak_count;
        }
    }

public:

    using elemet_type = T;

    WeakPtr(): control_block(nullptr), object(nullptr) {}

    template <typename Y>
    WeakPtr(const SharedPtr<Y>& shared_ptr): control_block(shared_ptr.control_block), object(shared_ptr.object) {
        increase_weak_count();
    }

    WeakPtr(const WeakPtr& other): control_block(other.control_block), object(other.object) {
        increase_weak_count();
    }

    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other): control_block(other.control_block), object(other.object) {
        increase_weak_count();
    }

    WeakPtr(WeakPtr&& other) noexcept : control_block(other.control_block), object(other.object) {
        other = nullptr;
    }

    template <typename Y>
    WeakPtr(WeakPtr<Y>&& other) noexcept : control_block(other.control_block), object(other.object) {
        other = nullptr;
    }

    WeakPtr& operator=(const WeakPtr& other) {
        WeakPtr(other).swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) {
        WeakPtr(other).swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const SharedPtr<Y>& other) {
        WeakPtr(other).swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        WeakPtr(std::move(other)).swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(WeakPtr<Y>&& other) noexcept {
        WeakPtr(std::move(other)).swap(*this);
        return *this;
    }

    ~WeakPtr() {

        if (control_block == nullptr) {
            return;
        }

        --control_block->weak_count;

        if (control_block->shared_count == 0 && control_block->weak_count == 0) {
            control_block->destroy();
        }

    }

    size_t use_count() const noexcept {
        return control_block->shared_count;
    }

    bool expired() const noexcept {
        return use_count() == 0;
    }

    SharedPtr<T> lock() const noexcept {
        return SharedPtr<T>(control_block, object);
    }

    void swap(WeakPtr& other) noexcept {
        std::swap(control_block, other.control_block);
        std::swap(object, other.object);
    }

};


template <typename T>
WeakPtr(SharedPtr<T>) -> WeakPtr<T>;


#endif // SHARED_PTR_H