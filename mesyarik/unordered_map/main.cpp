#include <unordered_map>
#include "unordered_map.h"
#include <iostream>
#include <type_traits>


struct A {
    std::pair<int, int> x;
    A() = default;
    A(const std::pair<int, int>& p): x(p) {};
    A(std::pair<int, int>&& p): x(std::move(p)) {};
};


struct B {
    A a;
    B(const A& a): a(a) {}
    B(A&& a): a(std::move(a)) {}
    B(B&&) = default;
    B& operator=(B&&) noexcept = default;
};


template <typename... Args>
void f(Args&&... args) {
    B b(A(std::pair(std::forward<Args>(args)...)));
}


struct S {

    int s;

    S(): s(0) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    S(int s): s(s) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    S(const S& other) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
        s = other.s;
    }

    S(S&& other) noexcept {
        std::cout << __PRETTY_FUNCTION__ << '\n';
        s = other.s;
    }

    S& operator=(const S& other) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
        s = other.s;
        return *this;
    }

    S& operator=(S&& other) noexcept {
        std::cout << __PRETTY_FUNCTION__ << '\n';
        s = other.s;
        return *this;
    }

    ~S() {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

};


template<>
struct std::hash<S> {
    std::size_t operator()(const S& s) const noexcept {
        return s.s;
    }
};


template<>
struct std::equal_to<S> {
    bool operator()(const S& lhs, const S& rhs) const noexcept {
        return lhs.s == rhs.s;
    }
};


// Finally, some tricky fixtures to test custom allocator.
// Done by professional, don't try to repeat
class Chaste {
private:
    int x = 0;
    Chaste(): x(0) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }
    Chaste(int x): x(x) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    // Nobody can construct me except this guy
    template<typename T>
    friend class TheChosenOne;

public:
    Chaste(const Chaste& other): x(other.x) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }
    Chaste(Chaste&& other): x(other.x) {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }
    ~Chaste() {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    bool operator==(const Chaste& other) const {
        return x == other.x;
    }
};


namespace std {
    template<>
    struct hash<Chaste> {
        size_t operator()(const Chaste& x) const noexcept {
            return std::hash<int>()(reinterpret_cast<const int&>(x));
        }
    };
}


template<typename T>
struct TheChosenOne: public std::allocator<T> {
    TheChosenOne() {}

    template<typename U>
    TheChosenOne(const TheChosenOne<U>&) {}

    template<typename... Args>
    void construct(T* p, Args&&... args) const {
        new(p) T(std::forward<Args>(args)...);
    }

    void construct(std::pair<const Chaste, Chaste>* p, int a, int b) const {
        new(p) std::pair<const Chaste, Chaste>(Chaste(a), Chaste(b));
    }

    void destroy(T* p) const {
        p->~T();
    }

    template<typename U>
    struct rebind {
        using other = TheChosenOne<U>;
    };
};


struct Node {

    Chaste chaste;
    int x;

    template <typename... Args>
    Node(Args&&... args, int x): chaste(std::forward<Args>(args)...), x(x) {}

    template <typename... Args>
    Node(std::piecewise_construct_t, Args&&... args, int x): chaste(std::forward<Args>(args)...), x(x) {}

    Node(const Chaste& chaste, int x): chaste(chaste), x(x) {}
    Node(Chaste&& chaste, int x): chaste(std::move(chaste)), x(x) {}

};


int main() {

    {
        UnorderedMap<S, S, std::hash<S>, std::equal_to<S>, TheChosenOne<std::pair<const S, S>>> m;
        m.emplace(0, 0);
    }
    std::cout << '\n';

    {
        std::unordered_map<S, S, std::hash<S>, std::equal_to<S>, TheChosenOne<std::pair<const S, S>>> m;
        m.emplace(0, 0);
    }
    std::cout << '\n';

    {
        UnorderedMap<Chaste, Chaste, std::hash<Chaste>, std::equal_to<Chaste>,
                TheChosenOne<std::pair<const Chaste, Chaste>>> m;
        m.emplace(0, 0);
    }
    std::cout << '\n';

    {
        std::unordered_map<Chaste, Chaste, std::hash<Chaste>, std::equal_to<Chaste>,
                TheChosenOne<std::pair<const Chaste, Chaste>>> m;
        m.emplace(0, 0);
    }
    std::cout << '\n';

//    {
//
//        if (std::is_constructible_v<Chaste, int>) {
//
//            TheChosenOne<Node> allocator;
//            auto ptr = std::allocator_traits<TheChosenOne<Node>>::allocate(allocator, 1);
//            std::allocator_traits<TheChosenOne<Node>>::construct(allocator, ptr, 0, 4);
//
//        } else {
//
//            TheChosenOne<Chaste> allocator;
//            auto ptr = std::allocator_traits<TheChosenOne<Chaste>>::allocate(allocator, 1);
//            std::allocator_traits<TheChosenOne<Chaste>>::construct(allocator, ptr, 0);
//
//            Node node(*ptr, 3);
//            std::cout << node.x << '\n';
//
//        }
//
//
//    }

}