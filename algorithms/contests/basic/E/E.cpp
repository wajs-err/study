#include <iostream>
#include <vector>


int main()
{

    std::cin.tie(0);
    std::ios_base::sync_with_stdio(0);

    size_t n, m, l;
    std::cin >> n >> m >> l;

    std::vector < std::vector <int> > a(n, std::vector <int> (l)), b(m, std::vector <int> (l));

    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < l; ++j)
            std::cin >> a[i][j];

    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < l; ++j)
            std::cin >> b[i][j];

    size_t q;
    std::cin >> q;

    for (size_t t = 0; t < q; ++t) {

        int i, j;
        std::cin >> i >> j;

        --i; --j;

        int left = 0, right = l - 1, mid;

        while (right - left > 1) {

            mid = (left + right) / 2;

            if (a[i][mid] - b[j][mid] < 0)
                left = mid;
            else
                right = mid;

        }

        std::cout << (std::max(a[i][left], b[j][left]) < std::max(a[i][right], b[j][right]) ? left + 1 : right + 1) << '\n';

    }

}