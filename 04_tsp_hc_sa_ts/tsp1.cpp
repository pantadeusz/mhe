#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "tsp_problem.hpp"

using namespace std; ///< bad practice, but useful in examples for MHE


/**
 * function that calculates accurate solution for TSP
 * */
// work_point_t brute_force_tsp(my_graph_t problem, std::function<double(work_point_t)> cost);
//  my_graph_t problem, function<double(work_point_t)> cost
auto brute_force_tsp = [](
                           auto cost,
                           auto generate_first_point,
                           auto next_point,
                           function<void(int c, double dt)> on_statistics = [](int c, double dt) {}) {
    int stat_goal_function_calls = 0;
    auto start = std::chrono::steady_clock::now();

    auto best_p = generate_first_point();
    double best_goal_val = cost(best_p);
    auto p = best_p; // current work point
    const auto p0 = p;
    do {
        const double cost_value = cost(p);
        stat_goal_function_calls++;
        if (cost_value < best_goal_val) {
            best_goal_val = cost_value;
            best_p = p;
            cout << "# better:  " << best_goal_val << endl;
        }
        p = next_point(p);
    } while (!(p == p0)); // the == operator must be defined for the
    auto finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = finish - start;
    on_statistics(stat_goal_function_calls, duration.count());
    return best_p;
};


auto hill_climb_rnd = [](
                          auto cost,
                          auto generate_first_point,
                          auto next_point,
                          int N,
                          function<void(int c, double dt)> on_statistics = [](int c, double dt) {}) {
    auto start = std::chrono::steady_clock::now();

    auto best_p = generate_first_point();
    double best_goal_val = cost(best_p);
    auto p = best_p; // current work point
    const auto p0 = p;
    for (int i = 0; i < N; i++) {
        p = next_point(best_p);
        const double cost_value = cost(p);
        if (cost_value < best_goal_val) {
            best_goal_val = cost_value;
            best_p = p;
            cout << "# better:  " << best_goal_val << endl;
        }
    }
    auto finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = finish - start;
    on_statistics(N, duration.count());
    return best_p;
};

int main(int argc, char** argv)
{
    // test_if_it_works();
    //    auto problem = load_problem("example1.txt");
    // auto problem = load_problem_from_coordinates({
    //    {0,1},
    //    {1,1},
    //    {1,2},
    //    {2,1},
    //    {1,0},
    //});
    vector<pair<double, double>> cities_coordinates;
    auto problem = generate_random_problem((argc < 2) ? 5 : stoi(argv[1]), [&cities_coordinates](auto coords) { cities_coordinates = coords; });
    cout << problem << endl;
    auto cost_func = cost_function_factory(problem);
    work_point_t sol_candidate = generate_random_tsp_point(problem);
    cout << "cost for: " << make_pair(problem, sol_candidate) << "is " << cost_func(sol_candidate) << endl;


    work_point_t best_solution = brute_force_tsp(
        cost_func,
        [&]() { return generate_first_tsp_point(problem); },
        get_next_point,
        [](int c, double dt) {
            cout << "# count BF: " << c << "; dt:  " << dt << endl;
        });

    work_point_t result_hc = hill_climb_rnd(
        cost_func,
        [&]() { return generate_random_tsp_point(problem); },
        generate_random_tsp_neighbour,
        10000,
        [](int c, double dt) {
            cout << "# count HC: " << c << "; dt:  " << dt << endl;
        });


    //    cout << "best cost for: " << make_pair(problem, best_solution) << "is " << cost_func(best_solution) << endl;
    cout << "best BF:\n"
         << make_pair(cities_coordinates, best_solution) << " is " << cost_func(best_solution) << endl;
    cout << "best HC:\n"
         << make_pair(cities_coordinates, result_hc) << " is " << cost_func(result_hc) << endl;

    return 0;
}