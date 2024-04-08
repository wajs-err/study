#include "matrix.h"

int main() {


    Matrix<4, 4> a = { {-1, -1, -1,  1},
                       { 1,  1,  0, -1},
                       {-1,  0,  0,  1},
                       { 0, -1,  0,  1} };

    std::cout << a.det();

}
