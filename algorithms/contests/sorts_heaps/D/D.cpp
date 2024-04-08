#include <iostream>
#include <string>


template <typename T>
class binary_heap {

public:

    binary_heap() {}

    ~binary_heap() {
        delete [] arr;
        delete [] index;
    }

    void insert(T val, int req) {
        curr++;
        arr[curr].first = val;
        arr[curr].second = req;
        index[req] = curr;
        sift_up(curr);
    }

    T get_min() {
        return arr[1].first;
    }

    void extract_min() {
        swap(1, curr);
        curr--;
        sift_down(1);
    }

    void decrease_key(int i, T delta) {
        arr[index[i]].first -= delta;
        sift_up(index[i]);
    }

private:

    std::pair<T /* value */, int /* index */>* arr = new std::pair <T, int> [100'001];
    int* index = new int [1'000'001]; // index[i] is position of ith element
    int curr = 0;

    void swap(int x, int y) {

        int tmp_ = index[arr[x].second];
        index[arr[x].second] = index[arr[y].second];
        index[arr[y].second] = tmp_;

        std::pair <T, int> tmp = arr[x];
        arr[x] = arr[y];
        arr[y] = tmp;

    }

    void sift_up(int v) {

        while (v != 1) {

            if (arr[v] < arr[v / 2]) {
                swap(v, v / 2);
                v /= 2;
            } else {
                break;
            }

        }

    }

    void sift_down(int v) {

        while (2 * v <= curr) {

            int u = 2 * v;

            if (2 * v  + 1 <= curr && arr[2 * v + 1] < arr[2 * v]) {
                u = 2 * v + 1;
            }

            if (arr[u] < arr[v]) {
                swap(u, v);
                v = u;
            } else {
                break;
            }

        }

    }

};


int main() {

    binary_heap<long long> h;

    int q;
    std::cin >> q;

    std::string s;
    for (int i = 0; i < q; ++i) {

        std::cin >> s;

        switch (s[0]) {

            case 'i':
                long long x;
                std::cin >> x;
                h.insert(x, i + 1);
                break;

            case 'g':
                std::cout << h.get_min() << '\n';
                break;

            case 'e':
                h.extract_min();
                break;

            case 'd':
                int _i;
                long long delta;
                std::cin >> _i >> delta;
                h.decrease_key(_i, delta);
                break;

        }

    }

}