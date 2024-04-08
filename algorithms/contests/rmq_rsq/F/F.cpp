#include <iostream>
#include <vector>

using std::vector;
using T = vector< vector < vector<int> > >;

int f(int x) {
    return x & (x + 1);
}


int g(int x) {
    return x | (x + 1);
}


int get_sum(int i_, int j_, int k_, T& t) {

    int res = 0;

    for (int i = i_; i >= 0; i = f(i) - 1) {
        for (int j = j_; j >= 0; j = f(j) - 1) {
            for (int k = k_; k >= 0; k = f(k) - 1) {
                res += t[i][j][k];
            }
        }
    }

    return res;

}


void update(int i_, int j_, int k_, int val, T& t, int n) {

    for (int i = i_; i <= n; i = g(i)) {
        for (int j = j_; j <= n; j = g(j)) {
            for (int k = k_; k <= n; k = g(k)) {
                t[i][j][k] += val;
            }
        }
    }

}


int main()
{

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    T a(n + 1, vector< vector<int> > (n + 1, vector<int> (n + 1)));
    T t(n + 1, vector< vector<int> > (n + 1, vector<int> (n + 1)));

    int m;
    std::cin >> m;
    while (m != 3) {

        if (m == 1) {

            int x, y, z, k;
            std::cin >> x >> y >> z >> k;
            update(x + 1, y + 1, z + 1, k, t, n);

        } else {

            int x1, y1, z1, x2, y2, z2;
            std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            ++x2; ++y2; ++z2;

            std::cout << get_sum(x2, y2, z2, t) - get_sum(x1, y1, z1, t)
                         - get_sum(x2, y2, z1, t) - get_sum(x2, y1, z2, t) - get_sum(x1, y2, z2, t)
                         + get_sum(x2, y1, z1, t) + get_sum(x1, y2, z1, t) + get_sum(x1, y1, z2, t) << '\n';

        }

        std::cin >> m;

    }

}