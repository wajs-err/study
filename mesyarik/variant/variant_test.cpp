#include <iostream>
#include <variant>
#include <vector>
#include <type_traits>
#include <cassert>

#include "variant.h"

//template <typename... Args>
//using Variant = std::variant<Args...>;

//template <typename T, typename... Types>
//using get = std::get<T, Types...>;

//using std::get;
//using std::holds_alternative;

void BasicTest() {

    Variant<int, std::string, double> v = 5;

    static_assert(std::is_assignable_v<decltype(v), float>);
    static_assert(!std::is_assignable_v<decltype(v), std::vector<int>>);

    assert(get<int>(v) == 5);

    v = "abc";

    assert(get<std::string>(v) == "abc");

    v = "cde";
    assert(get<std::string>(v) == "cde");

    v = 5.0;
    assert(get<double>(v) == 5.0);

    static_assert(!std::is_assignable_v<Variant<float, int>, double>);

    const auto& cv = v;
    static_assert(!std::is_assignable_v<decltype(get<double>(cv)), double>);

    static_assert(std::is_rvalue_reference_v<decltype(get<double>(std::move(v)))>);
    static_assert(std::is_lvalue_reference_v<decltype(get<double>(v))>);

}

void TestOverloadingSelection() {
    
    Variant<int*, char*, std::vector<char>, const int*, bool> v = true;

    assert(holds_alternative<bool>(v));

    v = std::vector<char>();

    get<std::vector<char>>(v).push_back('x');
    get<std::vector<char>>(v).push_back('y');
    get<std::vector<char>>(v).push_back('z');

    assert(holds_alternative<std::vector<char>>(v));

    assert(get<std::vector<char>>(v).size() == 3);

    char c = 'a';
    v = &c;

    assert(holds_alternative<char*>(v));

    *get<char*>(v) = 'b';

    assert(*get<char*>(v) == 'b');

    try {
        get<int*>(v);
        assert(false);
    } catch (...) {
        // ok
    }

    const int x = 1;
    v = &x;

    assert((!std::is_assignable_v<decltype(*get<const int*>(v)), int>));
    assert((std::is_assignable_v<decltype(get<const int*>(v)), int*>));
    
    try {
        get<int*>(v);
        assert(false);
    } catch (...) {
        // ok
    }

    const int y = 2;
    get<const int*>(v) = &y;
    assert(*get<const int*>(v) == 2);

    assert(!holds_alternative<int*>(v));
    assert(holds_alternative<const int*>(v));

    int z = 3;
    
    get<const int*>(v) = &z;
    assert(!holds_alternative<int*>(v));
    assert(holds_alternative<const int*>(v));
    assert(*get<const int*>(v) == 3);

    v = &z;

    assert(holds_alternative<int*>(v));
    assert(!holds_alternative<const int*>(v));
   
    assert(*get<int*>(v) == 3);
    
    assert((!std::is_assignable_v<decltype(get<int*>(v)), const int*>));

    try {
        get<const int*>(v);
        assert(false);
    } catch (...) {
        // ok
    }
}

void TestCopyMoveConstructorsAssignments() {
    
    Variant<std::string, char, std::vector<int>> v = "abcdefgh";

    auto vv = v;

    assert(get<std::string>(vv).size() == 8);
    assert(get<std::string>(v).size() == 8);

    {
        auto vvv = std::move(v);

        assert(get<std::string>(v).size() == 0);
        v.emplace<std::vector<int>>({1, 2, 3});

        assert(get<std::string>(vv).size() == 8);
    }

    v = std::move(vv);
    assert(get<std::string>(v).size() == 8);

    assert(get<std::string>(vv).size() == 0);

    vv = 'a';

    assert(holds_alternative<char>(vv));
    assert(holds_alternative<std::string>(v));

    get<0>(v).resize(3);
    get<0>(v)[0] = 'b';
    assert(get<std::string>(v) == "bbc");

    {
        Variant<int, const std::string> vvv = std::move(get<0>(v));

        std::vector<int> vec = {1, 2, 3, 4, 5};

        v = vec;
        assert(get<2>(v).size() == 5);
        assert(vec.size() == 5);

        vec[1] = 0;
        assert(get<std::vector<int>>(v)[1] == 2);

        vvv.emplace<int>(1);
        assert(holds_alternative<int>(vvv));
    }

}

void TestVariantWithConstType() {

    int& (*get_ptr)(Variant<int, double>&) = &get<int, int, double>;

    static_assert(!std::is_invocable_v<decltype(get_ptr), Variant<const int, double>>);

    const int& (*get_const_ptr)(Variant<const int, double>&) = &get<const int, const int, double>;
//8
    static_assert(!std::is_invocable_v<decltype(get_const_ptr), Variant<int, double>>);



    const int& (Variant<const int, double>::*method_const_ptr)(const int&) = &Variant<const int, double>::emplace<const int, const int&>;

    static_assert(!std::is_invocable_v<decltype(method_const_ptr), Variant<int, double>&, const int&>);

    int& (Variant<int, double>::*method_ptr)(const int&) = &Variant<int, double>::emplace<int, const int&>;

    static_assert(!std::is_invocable_v<decltype(method_ptr), Variant<const int, double>&, const int&>);

    

    Variant<const int, /*int,*/ std::string, const std::string, double> v = 1;
    
    //static_assert(!std::is_assignable_v<decltype(v), int>);

    assert(holds_alternative<const int>(v));

    v.emplace<std::string>("abcde");

    get<1>(v).resize(1);

    assert(!holds_alternative<const std::string>(v));


    v.emplace<0>(5);

    assert(get<0>(v) == 5);

}

struct VerySpecialType {};


int main() {

    std::cerr << "Tests started." << std::endl;

    BasicTest();
    std::cerr << "Test 1 (basic) passed." << std::endl;

    TestOverloadingSelection();
    std::cerr << "Test 2 (overloadings) passed." << std::endl;

    TestCopyMoveConstructorsAssignments();
    std::cerr << "Test 3 (copy-move ctors-assignments) passed." << std::endl;

    TestVariantWithConstType();
    std::cerr << "Test 4 (const types) passed." << std::endl;

    std::cerr << "Here should appear more tests later..." << std::endl;
    
    //TestValuelessByException();

    //TestVectorOfVariants();

    //TestVariantOfVariants();
    assert((!std::is_base_of_v<std::variant<VerySpecialType, int>, Variant<VerySpecialType, int>>));

}
