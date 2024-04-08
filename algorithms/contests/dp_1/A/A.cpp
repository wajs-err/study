// Thanks Artemy Novikov for this solution =)
// Btw problem is shit


#include <algorithm>
#include <iostream>
#include <vector>


using std::vector;
using std::string;
using std::pair;


int main() {

    int guys_count, girls_count;

    std::cin >> guys_count;
    vector<pair<int, string>> guys(guys_count);

    for (auto& el : guys) {
        std::cin >> el.second >> el.first;
    }

    std::cin >> girls_count;
    vector<pair<int, string>> girls(girls_count);

    for (auto& el : girls) {
        std::cin >> el.second >> el.first;
    }

    std::sort(guys.rbegin(), guys.rend());
    std::sort(girls.rbegin(), girls.rend());

    int max_taxi_count = (guys_count + girls_count + 3) / 4;

    vector<vector<pair<int/*cost*/, int/*guys in taxi*/>>>
            dp(guys_count + 1, vector<pair<int, int>>(max_taxi_count + 1, {0, 0}));

    int cost;
    for (int taxi = 0; taxi < max_taxi_count; ++taxi) {
        for (int guys_used = taxi; guys_used < guys_count && guys_used <= taxi * 4; ++guys_used) {

             cost = std::max(guys[guys_used].first,
                             taxi * 4 - guys_used < girls_count ? girls[taxi * 4 - guys_used].first : 0);

             for (int i = 1; i <= 3 && guys_used + i <= guys_count; ++i) {
                 if (dp[guys_used + i][taxi + 1].first == 0 ||
                            dp[guys_used][taxi].first + cost < dp[guys_used + i][taxi + 1].first) {
                     dp[guys_used + i][taxi + 1] = {dp[guys_used][taxi].first + cost, i};
                 }
             }

            if (guys_used + 4 <= guys.size() && (dp[guys_used + 4][taxi + 1].first == 0 ||
                        dp[guys_used][taxi].first + cost < dp[guys_used + 4][taxi + 1].first)) {
                dp[guys_used + 4][taxi + 1] = {dp[guys_used][taxi].first + guys[guys_used].first, 4};
            }

        }
    }

    auto taxi_count = max_taxi_count;
    auto price = dp.back().back().first;

    std::cout << price << '\n' << taxi_count << '\n';

    vector<vector<string>> taxis(taxi_count);

    int guys_remaining = guys_count;
    int girls_remaining = girls_count;
    int guys_in_taxi;
    for (auto taxi = taxi_count; taxi > 0; --taxi) {
        guys_in_taxi = dp[guys_remaining][taxi].second;
        for (int i = 0; i < guys_in_taxi; ++i) {
            --guys_remaining;
            taxis[taxi - 1].push_back(guys[guys_remaining].second);
        }
        for (int i = 0; i < girls_count - 4 * (taxi - 1) + guys_remaining && i < 4 - guys_in_taxi; ++i) {
            --girls_remaining;
            taxis[taxi - 1].push_back(girls[girls_remaining].second);
        }
    }

    for (int i = 0; i < taxi_count; ++i) {
        std::cout << "Taxi " << i + 1 << ": ";
        std::cout << taxis[i][0];
        for (int j = 1; j < taxis[i].size() - 1; ++j) {
            std::cout << ", " << taxis[i][j];
        }
        if (taxis[i].size() > 1) {
            std::cout << " and " << taxis[i].back();
        }
        std::cout << ".\n";
    }

}