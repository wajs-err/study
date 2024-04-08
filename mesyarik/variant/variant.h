#ifndef VARIANT_H
#define VARIANT_H


#include <type_traits>
#include <initializer_list>
#include <exception>
#include <numeric>
#include <memory>


struct bad_variant_access : public std::exception {
    const char* what() const noexcept override {
        return "bad variant access";
    }
};


template <typename... Types>
class Variant;


template <typename T, typename... Types>
T& get(Variant<Types...>&);


template <typename T, typename... Types>
const T& get(const Variant<Types...>&);


inline constexpr auto variant_npos = std::numeric_limits<size_t>::max();


namespace detail {


    template <size_t N, typename T, typename Head, typename... Tail>
    struct index_by_type_impl {
        static constexpr size_t value = std::is_same_v<T, Head> ? N : index_by_type_impl<N + 1, T, Tail...>::value;
    };


    // specialization for empty tail
    template <size_t N, typename T, typename Last>
    struct index_by_type_impl<N, T, Last> {
        static constexpr size_t value = std::is_same_v<T, Last> ? N : N + 1;
    };


    // works iff T in Types
    template <typename T, typename... Types>
    static constexpr size_t index_by_type = index_by_type_impl<0, T, Types...>::value;


    template <size_t N, typename Head, typename... Tail>
    struct type_by_index_impl {
        using type = std::conditional_t<N == 0, Head, typename type_by_index_impl<N - 1, Tail...>::type>;
    };


    template <size_t N, typename Head>
    struct type_by_index_impl<N, Head> {
        using type = Head;
    };


    // works iff N < size of Types...
    template <size_t N, typename... Types>
    using type_by_index = typename type_by_index_impl<N, Types...>::type;


    template <size_t N, typename T, typename Head, typename... Tail>
    struct index_if_constructible_impl {
        static constexpr size_t value = std::is_constructible_v<Head, T>
                    ? N
                    : index_if_constructible_impl<N + 1, T, Tail...>::value;
    };


    // specialization for empty tail
    template <size_t N, typename T, typename Last>
    struct index_if_constructible_impl<N, T, Last> {
        static constexpr size_t value = N;
    };


    template <typename T, typename... Types>
    static constexpr size_t index_if_constructible = index_if_constructible_impl<0, T, Types...>::value;


    template <size_t N, typename T, typename Head, typename... Tail>
    struct only_one_constructible_impl {
        static constexpr bool value = only_one_constructible_impl<
                N + static_cast<size_t>(std::is_constructible_v<Head, T>), T, Tail...>::value;
    };


    // specialization for empty tail
    template <size_t N, typename T, typename Last>
    struct only_one_constructible_impl<N, T, Last> {
        static constexpr bool value = N == 0 && std::is_constructible_v<Last, T>
                                   || N == 1 && !std::is_constructible_v<Last, T>;
    };


    template <typename T, typename... Types>
    static constexpr bool only_one_constructible = only_one_constructible_impl<0, T, Types...>::value;


    template <typename Head, typename... Tail>
    union VariadicUnion {

        Head head;
        VariadicUnion<Tail...> tail;

        VariadicUnion() {}
        ~VariadicUnion() {}

        template <size_t N>
        const auto& get() const noexcept {
            if constexpr (N == 0) {
                return head;
            } else {
                return tail.template get<N - 1>();
            }
        }

        template <size_t N>
        auto& get() noexcept {
            if constexpr (N == 0) {
                return head;
            } else {
                return tail.template get<N - 1>();
            }
        }

        template <typename T, typename... Args>
        void put(Args&&... args) {
            if constexpr (std::is_constructible_v<Head, Args...>) {
                ::new((void*) std::launder(&head)) T(std::forward<Args>(args)...);
            } else {
                tail.template put<T>(std::forward<Args>(args)...);
            }
        }

        template <typename T>
        void put() {
            if constexpr (std::is_same_v<T, Head>) {
                ::new((void*) std::launder(&head)) T();
            } else {
                tail.template put<T>();
            }
        }

        template <size_t N>
        void destroy() noexcept {
            if constexpr (N == 0) {
                head.~Head();
            } else {
                tail.template destroy<N - 1>();
            }
        }

    };


    // specialization for empty tail
    template <typename Head>
    union VariadicUnion<Head> {

        Head head;

        VariadicUnion() {}
        ~VariadicUnion() {}

        template <size_t N>
        const auto& get() const noexcept {
            static_assert(N == 0, "There is no alternative with such index");
            return head;
        }

        template <size_t N>
        auto& get() noexcept {
            static_assert(N == 0, "There is no alternative with such index");
            return head;
        }

        template <typename T, typename... Args>
        void put(Args&&... args) {
            static_assert(std::is_constructible_v<T, Args...>, "There is no alternative that can be constructed from the given arguments");
            ::new((void*) std::launder(&head)) T(std::forward<Args>(args)...);
        }

        template <typename T>
        void put() {
            static_assert(std::is_same_v<T, Head>, "There is no alternative that can be constructed from the given arguments");
            ::new((void*) std::launder(&head)) T();
        }

        template <size_t N>
        void destroy() noexcept {
            static_assert(N == 0, "Destroying variant went wrong...");
            head.~Head();
        }

    };


    template <typename... Types>
    struct VariantStorage {

        detail::VariadicUnion<Types...> storage;

        VariantStorage() = default;
        ~VariantStorage() = default;

    };


    template <typename T, typename... Types>
    struct VariantAlternative {

        using Derived = Variant<Types...>;

        static constexpr size_t index = index_by_type<T, Types...>;

        VariantAlternative() = default;
        ~VariantAlternative() = default;

        VariantAlternative(const T& value) {
            auto variant_ptr = static_cast<Derived*>(this);
            try {
                variant_ptr->storage.template put<T>(value);
            } catch (...) {
                variant_ptr->alternative_index = variant_npos;
                throw;
            }
            variant_ptr->alternative_index = index;
        }

        VariantAlternative(T&& value) noexcept {
            auto variant_ptr = static_cast<Derived*>(this);
            variant_ptr->alternative_index = index;
            variant_ptr->storage.template put<T>(std::move(value));
        }

        Derived& operator=(const T& value) {

            auto variant_ptr = static_cast<Derived*>(this);
            if (variant_ptr->alternative_index != index) {
                variant_ptr->destroy();
            }

            try {
                variant_ptr->storage.template put<T>(value);
            } catch (...) {
                if (variant_ptr->alternative_index != index) {
                    variant_ptr->alternative_index = variant_npos;
                }
                throw;
            }

            variant_ptr->alternative_index = index;
            return *variant_ptr;

        }

        Derived& operator=(T&& value) noexcept {

            auto variant_ptr = static_cast<Derived*>(this);
            if (variant_ptr->alternative_index != index) {
                variant_ptr->destroy();
            }
            variant_ptr->storage.template put<T>(std::move(value));
            variant_ptr->alternative_index = index;
            return *variant_ptr;

        }

        VariantAlternative(const VariantAlternative& other) {

            auto other_ptr = static_cast<const Derived*>(&other);
            auto ptr = static_cast<Derived*>(this);

            if (other.index == other_ptr->alternative_index) {

                try {
                    ptr->storage.template put<T>(other_ptr->storage.template get<index>());
                } catch (...) {
                    ptr->alternative_index = variant_npos;
                    throw;
                }

                ptr->alternative_index = index;

            }

        }

        VariantAlternative(VariantAlternative&& other) noexcept {

            auto other_ptr = static_cast<Derived*>(&other);
            auto ptr = static_cast<Derived*>(this);

            if (other.index == other_ptr->alternative_index) {
                ptr->storage.template put<T>(std::move(other_ptr->storage.template get<index>()));
                ptr->alternative_index = index;
            }

        }

        void destroy() noexcept {
            auto variant_ptr = static_cast<Derived*>(this);
            if (index == variant_ptr->alternative_index) {
                variant_ptr->storage.template destroy<index>();
            }
        }

    };


}


template <typename... Types>
class Variant
        : private detail::VariantStorage<Types...>,
          private detail::VariantAlternative<Types, Types...>... {

    static_assert(sizeof...(Types) > 0, "Variant must have at least one alternative");
    static_assert((!std::is_reference_v<Types>, ...), "Variant must have no reference alternative");
    static_assert((!std::is_void_v<Types>, ...), "Variant must have no void alternative");

    template <typename T, typename... Ts>
    friend struct detail::VariantAlternative;

    template <size_t N, typename... Ts>
    friend auto& get(Variant<Ts...>&);

    template <size_t N, typename... Ts>
    friend const auto& get(const Variant<Ts...>&);

    template <size_t N, typename... Ts>
    friend auto&& get(Variant<Ts...>&&);

    template <size_t N, typename... Ts>
    friend const auto&& get(const Variant<Ts...>&&);

    template <typename T, typename... Ts>
    friend bool holds_alternative(const Variant<Ts...>&) noexcept;

public:

    Variant(): alternative_index(0) {
        detail::VariantStorage<Types...>::storage.template put<detail::type_by_index<0, Types...>>();
    }

    ~Variant() {
        destroy();
    }

    using detail::VariantAlternative<Types, Types...>::VariantAlternative...;

    using detail::VariantAlternative<Types, Types...>::operator=...;

    Variant(const Variant& other)
            : detail::VariantStorage<Types...>(),
              detail::VariantAlternative<Types, Types...>(other)... {
        if (other.alternative_index == variant_npos) {
            alternative_index = variant_npos;
        }
    }

    Variant(Variant&& other) noexcept
            : detail::VariantStorage<Types...>(),
              detail::VariantAlternative<Types, Types...>(std::move(other))... {
        if (other.alternative_index == variant_npos) {
            alternative_index = variant_npos;
        }
    }

    template <typename T, typename = std::enable_if_t<detail::index_by_type<T, Types...> >= sizeof...(Types)
            && detail::only_one_constructible<T, Types...>>>
    Variant(T&& value) {
        static_assert(detail::only_one_constructible<T, Types...>,
                "There are more than one alternative that can be constructed from the given arguments");
        constexpr size_t new_index = detail::index_if_constructible<T, Types...>;
        try {
            detail::VariantStorage<Types...>::storage.
                    template put<detail::type_by_index<new_index, Types...>>(std::forward<T>(value));
        } catch (...) {
            alternative_index = variant_npos;
            throw;
        }
        alternative_index = new_index;
    }

    template <typename T, typename = std::enable_if_t<std::is_constructible_v<Variant, T>>>
    Variant& operator=(T&& value) {
        static_assert(detail::only_one_constructible<T, Types...>,
                "There are more than one alternative that can be constructed from the given arguments");
        constexpr size_t new_index = detail::index_if_constructible<T, Types...>;
        destroy_on_alternative_changing(new_index);
        try {
            detail::VariantStorage<Types...>::storage.
                    template put<detail::type_by_index<new_index, Types...>>(std::forward<T>(value));
        } catch (...) {
            alternative_index = variant_npos;
            throw;
        }
        alternative_index = new_index;
        return *this;
    }

    Variant& operator=(const Variant& other) {
        destroy_on_alternative_changing(other.alternative_index);
        (assign_correct_alternative(detail::VariantAlternative<Types, Types...>(), other), ...);
        return *this;
    }

    Variant& operator=(Variant&& other) noexcept {
        destroy_on_alternative_changing(other.alternative_index);
        (move_assign_correct_alternative(detail::VariantAlternative<Types, Types...>(), other), ...);
        other.destroy();
        return *this;
    }

    template <typename T, typename... Args>
    T& emplace(Args&&... args) {
        destroy();
        alternative_index = detail::index_by_type<T, Types...>;
        try {
            detail::VariantStorage<Types...>::storage.template put<T>(std::forward<Args>(args)...);
        } catch (...) {
            alternative_index = variant_npos;
            throw;
        }
        return get<T>(*this);
    }

    template <size_t N, typename... Args>
    auto& emplace(Args&&... args) {
        static_assert(N < sizeof...(Types), "The index must be in [0, number of alternatives)");
        return emplace<detail::type_by_index<N, Types...>>(std::forward<Args>(args)...);
    }

    template <typename T, typename U, typename... Args>
    T& emplace(std::initializer_list<U> list, Args&&... args) {
        destroy();
        alternative_index = detail::index_by_type<T, Types...>;
        try {
            detail::VariantStorage<Types...>::storage.template put<T>(list, std::forward<Args>(args)...);
        } catch (...) {
            alternative_index = variant_npos;
            throw;
        }
        return get<T>(*this);
    }

    template <size_t N, typename U, typename... Args>
    auto& emplace(std::initializer_list<U> list, Args&&... args) {
        static_assert(N < sizeof...(Types), "The index must be in [0, number of alternatives)");
        return emplace<detail::type_by_index<N, Types...>>(list, std::forward<Args>(args)...);
    }

    size_t index() const noexcept {
        return alternative_index;
    }

    bool valueless_by_exception() const noexcept {
        return alternative_index == variant_npos;
    }

private:

    size_t alternative_index;

    void destroy() noexcept {
        (detail::VariantAlternative<Types, Types...>::destroy(), ...);
    }

    void destroy_on_alternative_changing(size_t new_alternative) noexcept {

        if (new_alternative == variant_npos) {
            alternative_index = variant_npos;
            destroy();
        }

        if (new_alternative != alternative_index) {
            destroy();
        }

    }

    template <typename T>
    void assign_correct_alternative(const detail::VariantAlternative<T, Types...>& alternative,
                                    const Variant& other) {
        if (other.alternative_index == alternative.index) {
            try {
                detail::VariantStorage<Types...>::storage.
                        template put<detail::type_by_index<alternative.index, Types...>>(get<alternative.index>(other));
            } catch (...) {
                alternative_index = variant_npos;
                throw;
            }
            alternative_index = other.alternative_index;
        }
    }

    template <typename T>
    void move_assign_correct_alternative(const detail::VariantAlternative<T, Types...>& alternative,
                                         Variant& other) noexcept {
        if (other.alternative_index == alternative.index) {
            detail::VariantStorage<Types...>::storage.
                    template put<detail::type_by_index<alternative.index, Types...>>(
                    std::move(get<alternative.index>(other)));
            other.storage.template put<detail::type_by_index<alternative.index, Types...>>();
            alternative_index = other.alternative_index;
        }
    }

};


template <size_t N, typename... Types>
auto& get(Variant<Types...>& variant) {
    if (N == variant.alternative_index) {
        return variant.storage.template get<N>();
    } else {
        throw bad_variant_access();
    }
}


template <size_t N, typename... Types>
const auto& get(const Variant<Types...>& variant) {
    if (N == variant.alternative_index) {
        return variant.storage.template get<N>();
    } else {
        throw bad_variant_access();
    }
}


template <size_t N, typename... Types>
auto&& get(Variant<Types...>&& variant) {
    if (N == variant.alternative_index) {
        return variant.storage.template get<N>();
    } else {
        throw bad_variant_access();
    }
}


template <size_t N, typename... Types>
const auto&& get(const Variant<Types...>&& variant) {
    if (N == variant.alternative_index) {
        return variant.storage.template get<N>();
    } else {
        throw bad_variant_access();
    }
}


template <typename T, typename... Types>
T& get(Variant<Types...>& variant) {
    return get<detail::index_by_type<T, Types...>>(variant);
}


template <typename T, typename... Types>
const T& get(const Variant<Types...>& variant) {
    return get<detail::index_by_type<T, Types...>>(variant);
}


template <typename T, typename... Types>
T&& get(Variant<Types...>&& variant) {
    return get<detail::index_by_type<T, Types...>>(std::move(variant));
}


template <typename T, typename... Types>
const T&& get(const Variant<Types...>&& variant) {
    return get<detail::index_by_type<T, Types...>>(std::move(variant));
}


template <typename T, typename... Types>
bool holds_alternative(const Variant<Types...>& variant) noexcept {
    return variant.alternative_index == detail::index_by_type<T, Types...>;
}


#endif  // VARIANT_H