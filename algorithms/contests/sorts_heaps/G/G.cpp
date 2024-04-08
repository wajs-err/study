#include <iostream>


template <typename T>
class minmax_heap {

public:

    minmax_heap() {};

    ~minmax_heap() {
        delete [] min;
        delete [] max;
    }

    void insert(T val) {

        curr_min++;
        min[curr_min].first = val;
        min[curr_min].second = curr_min;

        curr_max++;
        max[curr_max].first = val;
        max[curr_max].second = curr_max;

        int index_max = sift_up_max(curr_max);
        int index_min = sift_up_min(curr_min);

        min[index_min].second = index_max;
        max[index_max].second = index_min;

    }

    T get_min() {
        T ret = min[1].first;
        extract_min();
        return ret;
    }

    T get_max() {
        T ret = max[1].first;
        extract_max();
        return ret;
    }

private:

    std::pair<T, int>* min = new std::pair <T, int> [100'001];
    int curr_min = 0;

    void extract_min() {

        int tmp = min[1].second;
        swap_max(min[1].second, curr_max);
        sift_up_max(tmp);
        curr_max--;

        swap_min(1, curr_min);
        curr_min--;
        sift_down_min(1);

    }

    void swap_min(int x, int y) {

        std::pair <T, int> tmp = min[x];
        min[x] = min[y];
        min[y] = tmp;

        int tmp_ = max[min[x].second].second;
        max[min[x].second].second = max[min[y].second].second;
        max[min[y].second].second = tmp_;

    }

    int sift_up_min(int v) {

        while (v != 1) {

            if (min[v] < min[v / 2]) {
                swap_min(v, v / 2);
                v /= 2;
            } else {
                return v;
            }

        }

        return v;

    }

    void sift_down_min(int v) {

        while (2 * v <= curr_min) {

            int u = 2 * v;

            if (2 * v  + 1 <= curr_min && min[2 * v + 1] < min[2 * v]) {
                u = 2 * v + 1;
            }

            if (min[u] < min[v]) {
                swap_min(u, v);
                v = u;
            } else {
                break;
            }

        }

    }

    std::pair<T, int>* max = new std::pair<T, int> [100'001];
    int curr_max = 0;

    void extract_max() {

        int tmp = max[1].second;
        swap_min(max[1].second, curr_min);
        sift_up_min(tmp);
        curr_min--;

        swap_max(1, curr_max);
        curr_max--;
        sift_down_max(1);

    }

    void swap_max(int x, int y) {

        std::pair <T, int> tmp = max[x];
        max[x] = max[y];
        max[y] = tmp;

        int tmp_ = min[max[x].second].second;
        min[max[x].second].second = min[max[y].second].second;
        min[max[y].second].second = tmp_;

    }

    int sift_up_max(int v) {

        while (v != 1) {

            if (max[v] > max[v / 2]) {
                swap_max(v, v / 2);
                v /= 2;
            } else {
                return v;
            }

        }

        return v;

    }

    void sift_down_max(int v) {

        while (2 * v <= curr_max) {

            int u = 2 * v;

            if (2 * v  + 1 <= curr_max && max[2 * v + 1] > max[2 * v]) {
                u = 2 * v + 1;
            }

            if (max[u] > max[v]) {
                swap_max(u, v);
                v = u;
            } else {
                break;
            }

        }

    }

};


int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    minmax_heap<int> mm;

    int q;
    std::cin >> q;

    for (int i = 0; i < q; ++i) {

        std::string s;
        std::cin >> s;

        if (s[0] == 'I') {
            mm.insert(stoi(s.substr(7, s.size() - 7)));
        } else if (s == "GetMin") {
            std::cout << mm.get_min() << ' ';
        } else if (s == "GetMax") {
            std::cout << mm.get_max() << ' ';
        }

    }

}