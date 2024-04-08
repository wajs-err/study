#include "string.h"
#include <fstream>

int main()
{

    std::fstream f("f.txt");

    String s;
    f >> s;
    std::cout << s;

}