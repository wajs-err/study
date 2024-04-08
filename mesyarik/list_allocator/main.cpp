#include <list>
#include <iostream>
#include "allocator.h"
#include "list.h"
#include <sys/resource.h>


int main() {

    using alloc = std::allocator<int>;

    alloc a;

    List<int> list_1;
    List<int, alloc> list_2(a);
    List<int, alloc> list_3(5, a);
    List<int, alloc> list_4(5, 6, a);
    List<int, alloc> list_5(5, 6);
    List<int, alloc> list_6(5);
    List<int, alloc> list_7(list_5);
    List<int, alloc> list_8;
    list_8 = list_7;

    List<int, alloc> list;

    int v = 7;

    list.push_back(5);
    list.push_back(v);
    list.push_front(5);
    list.push_front(v);

    list.emplace_back(3);
    list.emplace_front(3);

    auto it_1 = list.begin();   ++it_1; --it_1; it_1++; it_1--;
    auto it_2 = list.cbegin();  ++it_2; --it_2; it_2++; it_2--;
    auto it_3 = list.rbegin();  ++it_3; --it_3; it_3++; it_3--;
    auto it_4 = list.crbegin(); ++it_4; --it_4; it_4++; it_4--;

    auto it_5 = list.end();   ++it_5; --it_5; it_5++; it_5--;
    auto it_6 = list.cend();  ++it_6; --it_6; it_6++; it_6--;
    auto it_7 = list.rend();  ++it_7; --it_7; it_7++; it_7--;
    auto it_8 = list.crend(); ++it_8; --it_8; it_8++; it_8--;

    std::cerr << list.front() << ' ';
    std::cerr << list.back() << ' ';

    std::cerr << '\n';

    for (auto& el : list) {
        std::cerr << el << ' ';
    }

    std::cerr << '\n';

    auto it = list.begin();

    list.insert(it, 5);
    list.insert(it, v);

    list.emplace(it, 3);

    list.pop_back();
    list.pop_front();
    list.erase(it);

    std::cerr << list.size() << ' ' << list.max_size() << '\n';

    List<int, alloc> lst(list);

    std::cerr << (lst == list) << ' '
            << (lst != list) << ' '
            << (lst < list) << ' '
            << (lst > list) << ' '
            << (lst <= list) << ' '
            << (lst >= list) << ' ';

    std::cerr << '\n';

    swap(list, lst);
    lst.swap(list);

    std::cerr << (lst == list) << '\n';

}