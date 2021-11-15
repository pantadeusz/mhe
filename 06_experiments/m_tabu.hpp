#ifndef __M_TABU_____HPP___
#define __M_TABU_____HPP___


#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <set>

auto tabu_search = [](
                       auto cost,
                       auto generate_first_point,
                       auto neighbours_f, /// current work point neighbours
                       int N,
                       int tabu_size,
                       std::function<void(int c, double dt)> on_statistics = [](int c, double dt) {},
                       std::function<void(int i, double current_goal_val, double goal_val)> on_iteration = [](int i, double current_goal_val, double goal_val) {}) {
    using namespace std;
    auto start = chrono::steady_clock::now();

    auto best_p = generate_first_point();

    set<decltype(best_p)> taboo_set;
    list<decltype(best_p)> taboo_list;

    auto is_in_taboo = [&](auto e) {
        //if (taboo_set.count(e)) cerr << "tabu hit" << endl;
        return taboo_set.count(e); };
    auto add_to_taboo = [&](auto e) {
        taboo_set.insert(e);
        taboo_list.push_back(e);
    };
    auto shrink_taboo = [&]() {
        if (taboo_set.size() > tabu_size) {
            taboo_set.erase(taboo_list.front());
            taboo_list.pop_front();
            // cerr << "shrink list" << endl;
        }
    };

    auto p = best_p; // current work point
    for (int i = 0; i < N; i++) {
        auto neighbours = neighbours_f(best_p);
        neighbours.erase(std::remove_if(neighbours.begin(),
                             neighbours.end(),
                             [&](auto e) { return is_in_taboo(e); }),
            neighbours.end());
        if (neighbours.size() == 0) break;
        p = *max_element(neighbours.begin(), neighbours.end(), [&](auto a, auto b) {
            return cost(a) > cost(b);
        });
        add_to_taboo(p);
        const double cost_value = cost(p);
        if (cost(p) < cost(best_p)) {
            best_p = p;
        }
        shrink_taboo();
        on_iteration(i, cost(p), cost(best_p));
    }
    auto finish = chrono::steady_clock::now();
    chrono::duration<double> duration = finish - start;
    on_statistics(N, duration.count());
    return best_p;
};
#endif
