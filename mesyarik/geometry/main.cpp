#include "geometry.h"
#include <iomanip>

int main() {

    Point A(0, 0);
    Point B(0, 1);
    Point C(1, 1);
    Point D(1, 0);
    Point E(2, -1);

    Polygon t0(A, B, C, D, E);
    Polygon t1(A, B, C, E, D);

    std::cout << (t0 == t1) << '\n';

}