#ifndef ___M_HILLCLIMB_RANDOM___HPP___
#define ___M_HILLCLIMB_RANDOM___HPP___


#include <chrono>
#include <functional>


auto hill_climb_rnd = [](
                          auto cost,
                          auto generate_first_point,
                          auto next_point,
                          int N,
                          std::function<void(int c, double dt)> on_statistics = [](int c, double dt) {},
                          std::function<void(int i, double current_goal_val, double goal_val)>
                              on_iteration = [](int i, double current_goal_val, double goal_val) {}) {
    using namespace std;
    auto start = chrono::steady_clock::now();

    auto best_p = generate_first_point();
    double best_goal_val = cost(best_p);
    auto p = best_p; // current work point
    const auto p0 = p;
    for (int i = 0; i < N; i++) {
        on_iteration(i, cost(p), cost(best_p));
        p = next_point(best_p);
        const double cost_value = cost(p);
        if (cost_value < best_goal_val) {
            best_goal_val = cost_value;
            best_p = p;
        }
    }
    auto finish = chrono::steady_clock::now();
    chrono::duration<double> duration = finish - start;
    on_statistics(N, duration.count());
    return best_p;
};

#endif
