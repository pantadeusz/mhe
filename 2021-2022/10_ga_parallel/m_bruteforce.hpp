#ifndef ___M_BRUTEFORCE__HPP___
#define ___M_BRUTEFORCE__HPP___

#include <chrono>
#include <functional>

/**
 * function that calculates accurate solution for TSP
 * */
// work_point_t brute_force_tsp(my_graph_t problem, std::function<double(work_point_t)> cost);
//  my_graph_t problem, function<double(work_point_t)> cost
auto brute_force_tsp = [](
                           auto cost,
                           auto generate_first_point,
                           auto next_point,
                           std::function<void(int c, double dt)> on_statistics = [](int c, double dt) {},
                           std::function<void(int i, double current_goal_val, double goal_val)>
                               on_iteration = [](int i, double current_goal_val, double goal_val) {}) {
    using namespace std;
    int stat_goal_function_calls = 0;
    auto start = chrono::steady_clock::now();

    auto best_p = generate_first_point();
    double best_goal_val = cost(best_p);
    auto p = best_p; // current work point
    const auto p0 = p;
    int i = 0;
    do {
        const double cost_value = cost(p);
        stat_goal_function_calls++;
        if (cost_value < best_goal_val) {
            best_goal_val = cost_value;
            best_p = p;
        }
        p = next_point(p);
        on_iteration(i++, cost(p), cost(best_p));
    } while (!(p == p0)); // the == operator must be defined for the
    auto finish = chrono::steady_clock::now();
    chrono::duration<double> duration = finish - start;
    on_statistics(stat_goal_function_calls, duration.count());
    return best_p;
};


#endif
