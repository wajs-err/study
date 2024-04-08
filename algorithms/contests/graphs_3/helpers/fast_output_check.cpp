// This program causes memory leak...
// Here is Valgrind message:
//==32375== Memcheck, a memory error detector
//==32375== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
//==32375== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
//==32375== Command: ./a.out
//==32375==
//5
//==32375==
//==32375== HEAP SUMMARY:
//==32375==     in use at exit: 122,880 bytes in 6 blocks
//==32375==   total heap usage: 7 allocs, 1 frees, 195,584 bytes allocated
//==32375==
//==32375== LEAK SUMMARY:
//==32375==    definitely lost: 0 bytes in 0 blocks
//==32375==    indirectly lost: 0 bytes in 0 blocks
//==32375==      possibly lost: 0 bytes in 0 blocks
//==32375==    still reachable: 122,880 bytes in 6 blocks
//==32375==         suppressed: 0 bytes in 0 blocks
//==32375== Rerun with --leak-check=full to see details of leaked memory
//==32375==
//==32375== For lists of detected and suppressed errors, rerun with: -s
//==32375== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)



#include <iostream>


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout << 5 << '\n';
}