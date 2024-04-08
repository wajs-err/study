#include <iostream>
#include <vector>
#include <memory>
#include "smart_pointers.h"


struct A {};


struct B : A {};


int main() {

    if constexpr (std::is_convertible_v<B*, A*>) {}

    B b{};
    B* bp = new B;

    SharedPtr<A> asp(bp, std::default_delete<B>());

    SharedPtr<int> np;
    std::cout << np.use_count() << '\n';

    auto p = makeShared<std::string>(5, 'a');
    std::cout << *p << ' ' << p.use_count() << '\n';

    SharedPtr<std::string> pp(p);

    std::cout << *p << ' ' << p.use_count() << '\n';
    std::cout << *pp << ' ' << pp.use_count() << '\n';

    SharedPtr<std::string> ppp;
    ppp = pp;

    WeakPtr w(ppp);

    std::cout << *p << ' ' << p.use_count() << '\n';
    std::cout << *pp << ' ' << pp.use_count() << '\n';
    std::cout << *ppp << ' ' << ppp.use_count() << '\n';

}