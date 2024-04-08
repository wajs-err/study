#include <iostream>
#include <algorithm>
#include <vector>


int main()
{

    long long N;
    std::cin >> N;

    std::vector <int> L(N);
    std::vector <int> R(N);

    for (long long i = 0; i < N; ++i) {

        long long l, r;
        std::cin >> l >> r;

        L[i] = l;
        R[i] = r;

    }

    std::sort(L.begin(), L.end());
    std::sort(R.begin(), R.end());

    int current = 0, len = 0;
    long long l = 0, r = 0;

    for (long long i = L[0]; i <= R[N - 1] + 1; ++i) {

        while (l < N && L[l] == i) {
            current++;
            l++;
        }

        while (r < N && R[r] == i) {
            current--;
            r++;
        }

        if (current == 1)
            len++;

    }

    std::cout << len << '\n';

}