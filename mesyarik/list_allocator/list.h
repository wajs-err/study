#ifndef LIST_H
#define LIST_H


#include <memory>


template <typename T, typename Allocator = std::allocator<T>>
class List {

private:

    static_assert(std::is_same_v<std::remove_cv_t<T>, T>, "List must have non-const value type");
    static_assert(std::is_same_v<typename std::allocator_traits<Allocator>::value_type , T>,
                  "List must have the same value type as its allocator");

    struct BaseNode;
    struct Node;

    template <bool IsConst>
    class Iterator;

    using allocator_type = Allocator;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using node_allocator_type = typename allocator_traits::template rebind_alloc<Node>;
    using node_allocator_traits = std::allocator_traits<node_allocator_type>;

    using node_pointer = Node*;
    using base_node_pointer = BaseNode*;

public:

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    List(): fake_node(), allocator(), size_(0) {}

    List(const List& other)
            : fake_node(),
              allocator(node_allocator_traits::select_on_container_copy_construction(other.get_allocator())),
              size_(0) {

        const_iterator it = other.begin();
        try {
            for ( ; it != other.cend(); ++it) {
                push_back(*it);
            }
        } catch (...) {
            for (const_iterator i = other.begin(); i != it; ++i) {
                pop_back();
            }
            throw;
        }

    }

    List& operator=(const List& other) {
        List copy(other);
        BaseNode::swap(fake_node, copy.fake_node);
        std::swap(size_, copy.size_);
        if (node_allocator_traits::propagate_on_container_copy_assignment::value) {
            allocator = other.allocator;
        }
        return *this;
    }

    // move constructor and move assign operator are deleted, because I don't know how to implement them :(
    List(List&&) = delete;
    List& operator=(List&&) = delete;

    explicit List(const allocator_type& allocator)
            : fake_node(), allocator(allocator), size_(0) {}

    explicit List(size_type count, const allocator_type& allocator = allocator_type())
            : fake_node(), allocator(allocator), size_(0) {
        default_initialize(count);
    }

    explicit List(size_type count, const value_type& value, const allocator_type& allocator = allocator_type())
            : fake_node(), allocator(allocator), size_(0) {
        fill_initialize(count, value);
    }

    ~List() {
        clear();
    }

    template <typename... Args>
    iterator emplace(const_iterator iterator, Args&&... args) {
        base_node_pointer ptr = insert(iterator.node, std::forward<Args>(args)...);
        return ptr;
    }

    template <typename... Args>
    void emplace_front(Args&&... args) {
        insert(begin().node, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        insert(end().node, std::forward<Args>(args)...);
    }

    // inserts value before iterator position
    iterator insert(const_iterator iterator, const value_type& value) {
        base_node_pointer ptr = insert(iterator.node, value);
        return ptr;
    }

    void push_front(const value_type& value) {
        insert(begin().node, value);
    }

    void push_back(const value_type& value) {
        insert(end().node, value);
    }

    // inserts value before iterator position
    iterator insert(const_iterator iterator, value_type&& value) {
        base_node_pointer ptr = insert(iterator.node, std::move(value));
        return ptr;
    }

    void push_front(value_type&& value) {
        insert(begin().node, std::move(value));
    }

    void push_back(value_type&& value) {
        insert(end().node, std::move(value));
    }

    iterator erase(const_iterator iterator) noexcept {
        base_node_pointer node = iterator.node;
        node->untie_node();
        base_node_pointer next_node = node->next;
        node_allocator_traits::destroy(allocator, static_cast<node_pointer>(node));
        node_allocator_traits::deallocate(allocator, static_cast<node_pointer>(node), 1);
        --size_;
        return next_node;
    }

    void pop_front() noexcept {
        erase(begin());
    }

    void pop_back() noexcept {
        erase(--end());
    }

    allocator_type get_allocator() const noexcept {
        return typename node_allocator_traits::template rebind_alloc<value_type>(allocator);
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type max_size() const noexcept {
        return node_allocator_traits::max_size(allocator);
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void clear() noexcept {
        while (!empty()) {
            pop_back();
        }
    }

    reference front() noexcept {
        return *begin();
    }
    const_reference front() const noexcept {
        return *cbegin();
    }

    reference back() noexcept {
        return *--end();
    }
    const_reference back() const noexcept {
        return *--cend();
    }

    iterator begin() noexcept {
        return fake_node.next;
    }
    const_iterator begin() const noexcept {
        return fake_node.next;
    }
    const_iterator cbegin() const noexcept {
        return begin();
    }

    reverse_iterator rbegin() noexcept {
        return std::reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return std::reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    iterator end() noexcept {
        return &fake_node;
    }
    const_iterator end() const noexcept {
        return const_cast<base_node_pointer>(&fake_node);
    }
    const_iterator cend() const noexcept {
        return end();
    }

    reverse_iterator rend() noexcept {
        return std::reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return std::reverse_iterator(begin());
    }
    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    void swap(List& rhs) noexcept(allocator_traits::is_always_equal::value) {
        std::swap(size_, rhs.size_);
        BaseNode::swap(fake_node, rhs.fake_node);
        if (node_allocator_traits::propagate_on_container_swap::value) {
            std::swap(allocator, rhs.allocator);
        }
    }

private:

    BaseNode fake_node;
    node_allocator_type allocator;
    size_type size_;

    // inserts count new nodes initialized by value
    // can only be used for empty List
    void fill_initialize(size_type count, const_reference value) {

        size_type i = 0;

        try {
            for ( ; i < count; ++i) {
                push_back(value);
            }
        } catch (...) {
            for (size_type j = 0; j < i; ++j) {
                pop_back();
            }
        }

    }

    // inserts count new default initialized nodes
    // can only be used for empty List
    void default_initialize(size_type count) {

        size_type i = 0;

        try {
            for ( ; i < count; ++i) {
                emplace_back();
            }
        } catch (...) {
            for (size_type j = 0; j < i; ++j) {
                pop_back();
            }
        }

    }

    // inserts new node initialized by value before the given node
    template <typename... Args>
    base_node_pointer insert(base_node_pointer node, Args&&... args) {
        node_pointer ptr = node_allocator_traits::allocate(allocator, 1);
        try {
            node_allocator_traits::construct(allocator, ptr, std::forward<Args>(args)...);
        } catch (...) {
            node_allocator_traits::deallocate(allocator, ptr, 1);
            throw;
        }
        static_cast<base_node_pointer>(ptr)->tie_node(node);
        ++size_;
        return ptr;
    }

};


template <typename T, typename Allocator>
struct List<T, Allocator>::BaseNode {

    BaseNode(): prev(this), next(this) {}
    BaseNode(const BaseNode&) = delete;
    BaseNode& operator=(const BaseNode& other) = delete;
    BaseNode(BaseNode&&) = delete;
    BaseNode& operator=(BaseNode&&) = delete;
    ~BaseNode() = default;

    base_node_pointer prev;
    base_node_pointer next;

    static void swap(BaseNode& lhs, BaseNode& rhs) {

        // BaseNode::swap doesn't use move semantics

        if (rhs.next != &rhs && rhs.prev != &rhs) {
            rhs.next->prev = &lhs;
            rhs.prev->next = &lhs;
        }

        if (lhs.next != &lhs && lhs.prev != &lhs) {
            lhs.next->prev = &rhs;
            lhs.prev->next = &rhs;
        }

        base_node_pointer tmp = rhs.next;
        rhs.next = lhs.next;
        lhs.next = tmp;

        tmp = rhs.prev;
        rhs.prev = lhs.prev;
        lhs.prev = tmp;

    }

    // inserts BaseNode before next_node
    void tie_node(base_node_pointer next_node) noexcept {
        this->next = next_node;
        this->prev = next_node->prev;
        next_node->prev->next = this;
        next_node->prev = this;
    }

    // deletes BaseNode
    void untie_node() noexcept {
        this->prev->next = this->next;
        this->next->prev = this->prev;
    }

};


template <typename T, typename Allocator>
struct List<T, Allocator>::Node
        : public List<T, Allocator>::BaseNode {

    Node() = default;
    explicit Node(const value_type& value): BaseNode(), value(value) {}
    ~Node() = default;

    value_type value;

};


template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::Iterator {

public:

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
    using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
    using iterator_category = std::bidirectional_iterator_tag;

    Iterator(): node() {}
    Iterator(const Iterator&) = default;
    Iterator& operator=(const Iterator& iterator) = default;
    ~Iterator() = default;

    reference operator*() const noexcept {
        return static_cast<node_pointer>(node)->value;
    }

    pointer operator->() const noexcept {
        return &static_cast<node_pointer>(node)->value;
    }

    Iterator& operator++() noexcept {
        node = node->next;
        return *this;
    }

    Iterator operator++(int) noexcept {
        Iterator copy = *this;
        node = node->next;
        return copy;
    }

    Iterator& operator--() noexcept {
        node = node->prev;
        return *this;
    }

    Iterator operator--(int) noexcept {
        Iterator copy = *this;
        node = node->prev;
        return copy;
    }

    operator Iterator<true>() const noexcept {
        return node;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) noexcept {
        return lhs.node == rhs.node;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) noexcept {
        return lhs.node != rhs.node;
    }

private:

    friend class List;

    Iterator(base_node_pointer node): node(node) {}

    base_node_pointer node;

};


template <typename T, typename Allocator>
void swap(List<T, Allocator>& lhs, List<T, Allocator>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}


template <typename T, typename Allocator>
bool operator==(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs) {

    if (lhs.size() != rhs.size()) {
        return false;
    }

    typename List<T, Allocator>::const_iterator lhs_iterator = lhs.begin();
    typename List<T, Allocator>::const_iterator rhs_iterator = rhs.begin();

    typename List<T, Allocator>::const_iterator lhs_end = lhs.end();
    typename List<T, Allocator>::const_iterator rhs_end = rhs.end();

    while (lhs_iterator != lhs_end && rhs_iterator != rhs_end && *lhs_iterator == *rhs_iterator) {
        ++lhs_iterator;
        ++rhs_iterator;
    }

    return lhs_iterator == lhs_end && rhs_iterator == rhs_end;

}


template <typename T, typename Allocator>
bool operator!=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs) {
    return !(lhs == rhs);
}


template <typename T, typename Allocator>
bool operator<(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}


template <typename T, typename Allocator>
bool operator>(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs) {
    return rhs < lhs;
}


template <typename T, typename Allocator>
bool operator<=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs) {
    return !(rhs < lhs);
}


template <typename T, typename Allocator>
bool operator>=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs) {
    return !(lhs < rhs);
}


#endif // LIST_H