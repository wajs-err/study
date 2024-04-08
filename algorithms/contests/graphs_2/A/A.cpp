#include <iostream>
#include <vector>
#include <queue>
#include <numeric>


using std::vector;
using std::queue;


class Graph {

private:

    const int inf = std::numeric_limits<int>::max();

    size_t size;
    vector<vector<int>> g;
    vector<int> dist;

    void bfs(int s, int t) {

        dist[s] = 0;
        queue<int> q;
        q.push(s);

        int v;
        while (!q.empty()) {

            if (dist[t] != inf) {
                return;
            }

            v = q.front();
            q.pop();

            for (int& to : g[v]) {
                if (dist[to] == inf) {
                    dist[to] = dist[v] + 1;
                    q.push(to);
                }
            }

        }

    }

public:

    Graph() = delete;
    Graph(size_t size): size(size), g(vector<vector<int>>(size)), dist(vector<int>(size, inf)) {}
    ~Graph() = default;

    void push_edge(int u, int v) {
        g[u].push_back(v);
    }

    vector<int> find_path(int s, int t) {

        vector<int> path;

        bfs(s, t);

        int v = t;
        path.push_back(v);
        while (dist[v] != 0) {

            for (int &from: g[v]) {
                if (dist[from] < dist[v]) {
                    v = from;
                    path.push_back(from);
                    continue;
                }
            }

        }

        std::reverse(path.begin(), path.end());
        return path;

    }

};


int main() {

    Graph g(64);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {

            if (i - 1 >= 0 && j + 2 < 8) {
                g.push_edge(i * 8 + j, (i - 1) * 8 + (j + 2));
            }
            if (i + 1 < 8 && j + 2 < 8) {
                g.push_edge(i * 8 + j, (i + 1) * 8 + (j + 2));
            }
            if (i - 1 >= 0 && j - 2 >= 0) {
                g.push_edge(i * 8 + j, (i - 1) * 8 + (j - 2));
            }
            if (i + 1 < 8 && j - 2 >= 0) {
                g.push_edge(i * 8 + j, (i + 1) * 8 + (j - 2));
            }
            if (i + 2 < 8 && j - 1 >= 0) {
                g.push_edge(i * 8 + j, (i + 2) * 8 + (j - 1));
            }
            if (i + 2 < 8 && j + 1 < 8) {
                g.push_edge(i * 8 + j, (i + 2) * 8 + (j + 1));
            }
            if (i - 2 >= 0 && j - 1 >= 0) {
                g.push_edge(i * 8 + j, (i - 2) * 8 + (j - 1));
            }
            if (i - 2 >= 0 && j + 1 < 8) {
                g.push_edge(i * 8 + j, (i - 2) * 8 + (j + 1));
            }

        }
    }

    int s, t;
    std::string str;
    std::cin >> str;
    s = (str[0] - 'a') * 8 + (str[1] - '1');
    std::cin >> str;
    t = (str[0] - 'a') * 8 + (str[1] - '1');

    for (int& el : g.find_path(s, t)) {
        std::cout << static_cast<char>(el / 8 + 'a') << el % 8 + 1 << '\n';
    }

}