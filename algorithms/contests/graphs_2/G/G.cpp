#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <numeric>


using std::vector;
using std::set;
using std::string;
using std::queue;
using std::pair;


struct Lift {

    long long x;
    long long y;
    inline static long long b;
    inline static long long c;

    Lift(long long x, long long y): x(x), y(y) {}
    Lift() = default;
    ~Lift() = default;

};


bool operator<(const Lift& lhs, const Lift& rhs) {
    return Lift::b * lhs.x + Lift::c * lhs.y < Lift::b * rhs.x + Lift::c * rhs.y;
}

constexpr auto inf = std::numeric_limits<long long>::max();


vector<long long> Dijkstra(long long h, long long a, long long b, long long c) {

    Lift::b = b;
    Lift::c = c;

    vector dist(a, inf);
    vector used_mod_a(a, false);

    set<Lift> unused;
    unused.insert(Lift());

    while (!unused.empty()) {

        auto p = *unused.begin();
        unused.erase(unused.begin());
        used_mod_a[(Lift::b * p.x + Lift::c * p.y + 1) % a] = true;
        dist[(Lift::b * p.x + Lift::c * p.y + 1) % a]
                = std::min(dist[(Lift::b * p.x + Lift::c * p.y + 1) % a], Lift::b * p.x + Lift::c * p.y + 1);

        if (!used_mod_a[(Lift::b * p.x + Lift::c * (p.y + 1) + 1) % a]
            && dist[(Lift::b * p.x + Lift::c * (p.y + 1) + 1) % a] > Lift::b * p.x + Lift::c * (p.y + 1) + 1
            && Lift::b * p.x + Lift::c * (p.y + 1) + 1 <= h) {
            unused.insert(Lift(p.x, p.y + 1));
        }

        if (!used_mod_a[(Lift::b * (p.x + 1) + Lift::c * p.y + 1) % a]
            && dist[(Lift::b * (p.x + 1) + Lift::c * p.y + 1) % a] > Lift::b * (p.x + 1) + Lift::c * p.y + 1
            && Lift::b * (p.x + 1) + Lift::c * p.y + 1 <= h) {
            unused.insert(Lift(p.x + 1, p.y));
        }

    }

    return dist;

}


int main() {

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    long long h;
    long long a, b, c;

    std::cin >> h >> a >> b >> c;

    vector v = {a, b, c};
    std::sort(v.begin(), v.end());
    a = v[0];
    b = v[1];
    c = v[2];

    long long res = 0;

    for (auto& el : Dijkstra(h, a, b, c)) {
        if (el != inf) {
            res += (h - el) / a + 1;
        }
    }

    std::cout << res << '\n';

}