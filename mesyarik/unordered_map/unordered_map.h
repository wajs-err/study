#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H


#include "list.h"
#include <functional>
#include <exception>
#include <vector>
#include <cmath>


template <typename Key, typename Value,
        typename Hash = std::hash<Key>,
        typename KeyEqual = std::equal_to<Key>,
        typename Allocator = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {

public:

    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using size_type = std::size_t;
    using allocator_type = Allocator;
    using difference_type = std::ptrdiff_t;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using NodeType = value_type;

private:

    using hash_return_type = std::size_t;

    struct ListNode {

        NodeType node;
        hash_return_type cached_hash;

        ListNode(const NodeType& node): node(node), cached_hash(hasher()(node.first)) {}

        ListNode(NodeType&& node): node(std::move(node)), cached_hash(hasher()(node.first)) {}

        template <typename... Args>
        ListNode(Args&&... args): node(std::forward<Args>(args)...), cached_hash(hasher()(node.first)) {}

        ~ListNode() = default;

    };

    using allocator_traits = std::allocator_traits<allocator_type>;

    using list_node_allocator_type = typename allocator_traits::template rebind_alloc<ListNode>;
    using list_type = List<ListNode, list_node_allocator_type>;

    using list_iterator_type = typename list_type::iterator;
    using list_iterator_allocator_type = typename allocator_traits::template rebind_alloc<list_iterator_type>;
    using vector_type = std::vector<list_iterator_type, list_iterator_allocator_type>;

    list_type buckets;
    vector_type bucket_iterators;
    allocator_type allocator;
    size_type size_;
    size_type bucket_count_;
    float max_load_factor_;

    template <bool IsConst>
    class CommonIterator;

    bool is_new_load_factor_correct(size_type new_size) const noexcept {
        return bucket_count_ != 0 && new_load_factor(new_size) <= max_load_factor_;
    }

    float new_load_factor(size_type new_size) const noexcept {
        return static_cast<float>(new_size) / static_cast<float>(bucket_count_);
    }

    void update_end_iterators(list_iterator_type new_end, list_iterator_type old_end) noexcept {
        std::for_each(bucket_iterators.begin(), bucket_iterators.end(),
                      [new_end, old_end](auto& el){
                          if (el == old_end) {
                              el = new_end;
                          }
                      });
    }

    void set_new_bucket_iterators(const UnorderedMap& other) noexcept {

        bucket_iterators[buckets.begin()->cached_hash % bucket_count_] = buckets.begin();

        for (auto it = ++buckets.begin(); it != buckets.end(); ++it) {

            if (std::prev(it)->cached_hash != it->cached_hash) {
                bucket_iterators[it->cached_hash % bucket_count_] = it;
            }

        }

        update_end_iterators(buckets.end(), const_cast<UnorderedMap&>(other).buckets.end());

    }

public:

    using Iterator = CommonIterator<false>;
    using ConstIterator = CommonIterator<true>;

    UnorderedMap(): buckets(), bucket_iterators(), allocator(), size_(0), bucket_count_(0), max_load_factor_(1.0) {}

    UnorderedMap(size_type bucket_count)
            : buckets(), bucket_iterators(bucket_count, buckets.end()), allocator(), size_(0),
              bucket_count_(bucket_count), max_load_factor_(1.0) {}

    UnorderedMap(const UnorderedMap& other)
            : buckets(other.buckets), bucket_iterators(other.bucket_iterators),
              allocator(allocator_traits::select_on_container_copy_construction(other.allocator)), size_(other.size_),
              bucket_count_(other.bucket_count_), max_load_factor_(other.max_load_factor_) {

        set_new_bucket_iterators(other);

    }

    UnorderedMap(UnorderedMap&& other) = default;

    UnorderedMap& operator=(const UnorderedMap& other) {

        buckets = other.buckets;
        bucket_iterators = other.bucket_iterators;
        if constexpr (allocator_traits::propagate_on_container_copy_assignment::value) {
            allocator = other.allocator;
        }
        size_ = other.size_;
        bucket_count_ = other.bucket_count_;
        max_load_factor_ = other.max_load_factor_;

        set_new_bucket_iterators(other);

        return *this;

    }

    UnorderedMap& operator=(UnorderedMap&& other) noexcept {

        if (this == &other) {
            return *this;
        }

        buckets = std::move(other.buckets);
        bucket_iterators = std::move(other.bucket_iterators);
        if constexpr (allocator_traits::propagate_on_container_copy_assignment::value) {
            allocator = std::move(other.allocator);
        }
        size_ = other.size_; // move is useless for size_t and float
        bucket_count_ = other.bucket_count_;
        max_load_factor_ = other.max_load_factor_;

        set_new_bucket_iterators(other);

        return *this;

    }

    ~UnorderedMap() = default;

    std::pair<Iterator, bool> insert(const NodeType& node) {
        auto p = emplace(node);
        return p;
    }

    std::pair<Iterator, bool> insert(NodeType&& node) {
        auto p = emplace(std::move(node));
        return p;
    }

    template <typename P>
    std::enable_if_t<std::is_constructible_v<NodeType, P>, std::pair<Iterator, bool>> insert(P&& node) {
        auto p = emplace(std::forward<P>(node));
        return p;
    }

    template <typename InputIterator>
    void insert(InputIterator begin, InputIterator end) {
        for (auto it = begin; it != end; ++it) {
            insert(*it);
        }
    }

    template <typename... Args>
    std::pair<Iterator, bool> emplace(Args&&... args) {

        if constexpr (std::is_constructible_v<value_type, Args...>) {
            typename list_type::node_pointer list_node = buckets.template create_node(std::forward<Args>(args)...);
            return insert_list_node(list_node);
        } else {
            auto ptr = allocator_traits::allocate(allocator, 1);
            try {
                allocator_traits::construct(allocator, ptr, std::forward<Args>(args)...);
            } catch (...) {
                allocator_traits::deallocate(allocator, ptr, 1);
                throw;
            }
            typename list_type::node_pointer list_node = buckets.template create_node(std::move(*ptr));
            allocator_traits::destroy(allocator, ptr);
            allocator_traits::deallocate(allocator, ptr, 1);
            return insert_list_node(list_node);
        }

    }

    Iterator erase(ConstIterator iterator) noexcept {

        auto list_iterator = iterator.list_iterator;

        if ((*std::prev(list_iterator)).cached_hash != (*list_iterator).cached_hash) {

            if (std::next(list_iterator) == buckets.end()) {

                bucket_iterators[(*list_iterator).cached_hash % bucket_count_] = buckets.end();

            } else {

                bucket_iterators[(*list_iterator).cached_hash % bucket_count_] =
                        (*std::next(list_iterator)).cached_hash != (*list_iterator).cached_hash
                        ? buckets.end() : std::next(list_iterator);

            }

        }

        --size_;
        auto it = buckets.erase(list_iterator);
        return it;

    }

    Iterator erase(ConstIterator begin, ConstIterator end) noexcept {
        ConstIterator it_;
        for (auto it = begin; it != end; ) {
            it_ = std::next(it);
            erase(it);
            it = it_;
        }
        return static_cast<Iterator>(end.list_iterator);
    }

    size_type erase(const Key& key) noexcept {
        auto it = find(key);
        if (it == end()) {
            return 0;
        }
        erase(it);
        return 1;
    }

    mapped_type& operator[](const Key& key) noexcept {

        auto [iterator, is_inserted] = emplace(std::piecewise_construct,
                                               std::forward_as_tuple(key),
                                               std::tuple<>());
        return iterator->second;

    }

    mapped_type& operator[](Key&& key) noexcept {

        auto [iterator, is_inserted] = emplace(std::piecewise_construct,
                                               std::forward_as_tuple(std::move(key)),
                                               std::tuple<>());
        return iterator->second;

    }

    mapped_type& at(const Key& key) {
        return const_cast<mapped_type&>(const_cast<const UnorderedMap*>(this)->at(key));
    }

    const mapped_type& at(const Key& key) const {

        auto it = find(key);

        if (it == end()) {
            throw std::out_of_range("There is no element with the given key in the container");
        }

        return it->second;

    }

    Iterator find(const Key& key) {

        if (size_ == 0 || bucket_count_ == 0) {
            return end();
        }

        hash_return_type hash = hasher()(key);
        auto it = bucket_iterators[hash % bucket_count_];

        if (it == buckets.end()) {
            return buckets.end();
        }

        while (it != end() && it->cached_hash % bucket_count_ == hash % bucket_count_) {
            if (key_equal()(it->node.first, key)) {
                return it;
            }
            ++it;
        }

        return buckets.end();

    }

    ConstIterator find(const Key& key) const {
        return const_cast<UnorderedMap *>(this)->find(key);
    }

    void reserve(size_type new_bucket_count) {
        if (new_bucket_count > bucket_count_) {
            rehash(std::ceil(static_cast<float>(new_bucket_count) / max_load_factor_));
        }
    }

    void rehash(size_type new_bucket_count) {

        if (new_bucket_count < static_cast<size_type>(static_cast<float>(size_) / max_load_factor_)) {
            new_bucket_count = static_cast<size_type>(static_cast<float>(size_) / max_load_factor_);
        }

        list_type new_buckets;

        bucket_iterators.resize(new_bucket_count);
        std::for_each(bucket_iterators.begin(), bucket_iterators.end(), [&new_buckets](auto& el){
            el = new_buckets.end();
        });
        bucket_count_ = new_bucket_count;

        typename list_type::iterator it_;
        for (auto it = buckets.begin(); it != buckets.end(); ) {

            it_ = std::next(it);

            if (bucket_iterators[(*it).cached_hash % bucket_count_] == new_buckets.end()) {
                new_buckets.splice(new_buckets.begin(), buckets, it);
                bucket_iterators[(*it).cached_hash % bucket_count_] = new_buckets.begin();
            } else {
                new_buckets.splice(bucket_iterators[(*it).cached_hash % bucket_count_], buckets, it);
                --bucket_iterators[(*it).cached_hash % bucket_count_];
            }

            it = it_;

        }

        buckets = std::move(new_buckets);

        update_end_iterators(buckets.end(), new_buckets.end());

    }

    allocator_type get_allocator() const noexcept {
        return allocator;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type max_size() const noexcept {
        return buckets.max_size();
    }

    float load_factor() const noexcept {
        return bucket_count_ == 0
                ? static_cast<float>(bucket_count_)
                : static_cast<float>(size_) / static_cast<float>(bucket_count_);
    }

    float max_load_factor() const noexcept {
        return max_load_factor_;
    }

    void max_load_factor(float new_max_load_factor) noexcept {
        max_load_factor_ = new_max_load_factor;
        if (load_factor() > max_load_factor_) {
            rehash(static_cast<size_type>(static_cast<float>(size_) / max_load_factor_));
        }
    }

    size_type bucket_count() const noexcept {
        return bucket_count_;
    }

    size_type max_bucket_count() const noexcept {
        return bucket_iterators.max_size();
    }

    Iterator begin() noexcept {
        return buckets.begin();
    }

    ConstIterator begin() const noexcept {
        return const_cast<UnorderedMap*>(this)->begin();
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    Iterator end() noexcept {
        return buckets.end();
    }

    ConstIterator end() const noexcept {
        return const_cast<UnorderedMap*>(this)->end();
    }

    ConstIterator cend() const noexcept {
        return end();
    }

private:

    std::pair<Iterator, bool> insert_list_node(typename list_type::node_pointer& list_node) {

        auto it = find(list_node->value.node.first);
        if (it != end()) {
            return {it, false};
        }

        if (!is_new_load_factor_correct(size_ + 1)) {
            rehash(bucket_count_ == 0 ? 1 : 2 * bucket_count_);
        }

        auto it_ = bucket_iterators[list_node->value.cached_hash % bucket_count_];

        if (it_ == buckets.end()) {
            buckets.insert_node(list_node, buckets.begin());
            ++size_;
            bucket_iterators[list_node->value.cached_hash % bucket_count_] = buckets.begin();
            return {Iterator(buckets.begin()), true};
        }

        buckets.insert_node(list_node, it_);
        ++size_;
        bucket_iterators[list_node->value.cached_hash % bucket_count_] = --it_;
        return {Iterator(it_), true};

    }

};


template <typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
template <bool IsConst>
class UnorderedMap<Key, Value, Hash, KeyEqual, Allocator>::CommonIterator {

public:

    using difference_type = std::ptrdiff_t;
    using value_type = NodeType;
    using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
    using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
    using iterator_category = std::forward_iterator_tag;

    CommonIterator() = default;
    CommonIterator(const CommonIterator&) = default;
    CommonIterator& operator=(const CommonIterator&) = default;
    CommonIterator(CommonIterator&&) noexcept = default;
    CommonIterator& operator=(CommonIterator&&) noexcept = default;
    ~CommonIterator() = default;

    reference operator*() const noexcept {
        return list_iterator->node;
    }

    pointer operator->() const noexcept {
        return &(list_iterator->node);
    }

    CommonIterator& operator++() noexcept {
        ++list_iterator;
        return *this;
    }

    CommonIterator operator++(int) noexcept {
        Iterator copy = *this;
        ++list_iterator;
        return copy;
    }

    friend bool operator==(const CommonIterator& lhs, const CommonIterator& rhs) {
        return lhs.list_iterator == rhs.list_iterator;
    }

    friend bool operator!=(const CommonIterator& lhs, const CommonIterator& rhs) {
        return !(lhs == rhs);
    }

    operator CommonIterator<true>() const noexcept {
        return list_iterator;
    }

private:

    friend class UnorderedMap;

    list_iterator_type list_iterator;

    CommonIterator(list_iterator_type list_iterator): list_iterator(list_iterator) {}

};


#endif  // UNORDERED_MAP_H