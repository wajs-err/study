#include <stack>
#include <cstdio>
#include <iostream>

int main()
{

    std::stack < std::pair <long long, int> > hist;
    hist.push( std::make_pair(0ll, 1) );

    size_t n;
    scanf("%zu", &n);

    long long mx = 0;

    for (size_t i = 0; i <= n; ++i) {

        long long h = 0;

        if (i < n)
            scanf("%lld", &h);

        if (h >= hist.top().first)
            hist.push( std::make_pair(h, 1) );
        else {

            int j = 0;

            while (hist.top().first > h) {

                j += hist.top().second;

                if (j * hist.top().first > mx)
                    mx = j * hist.top().first;

                hist.pop();

            }

            hist.push( std::make_pair(h, j + 1) );

        }

    }

    printf("%lld", mx);

}