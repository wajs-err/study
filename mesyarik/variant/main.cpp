#include "variant.h"
#include <iostream>
#include <vector>
#include <variant>


void f(const int) {

}


void f(double) {

}


int main() {

    std::variant<int, const int> sv;

    std::cout << std::boolalpha;

    Variant<const int, double> v = 1;

    std::cout << holds_alternative<const int>(v) << '\n';

    Variant<const int, std::string> vv = "dfghj";

    get<std::vector<int>>(vv);

}