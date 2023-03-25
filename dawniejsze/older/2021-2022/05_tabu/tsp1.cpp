#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

// different methods
#include "m_bruteforce.hpp"
#include "m_hillclimb.hpp"
#include "m_tabu.hpp"

// problem definition
#include "tsp_problem.hpp"

using namespace std; ///< bad practice, but useful in examples for MHE

// the experiment
int main(int argc, char** argv)
{
    vector<pair<double, double>> cities_coordinates;
    auto problem = generate_random_problem((argc < 2) ? 5 : stoi(argv[1]), [&cities_coordinates](auto coords) { cities_coordinates = coords; });
    cout << problem << endl;
    auto cost_func = cost_function_factory(problem);

    int iterations = 10000000;

    // work_point_t best_solution = brute_force_tsp(
    //     cost_func,
    //     [&]() { return generate_first_tsp_point(problem); },
    //     get_next_point,
    //     [](int c, double dt) {
    //         cout << "# count BF: " << c << "; dt:  " << dt << endl;
    //     });

    auto start_point = generate_random_tsp_point(problem);
    work_point_t result_hc = hill_climb_rnd(
        cost_func,
        [&]() { return start_point; },
        generate_random_tsp_neighbour,
        iterations,
        [](int c, double dt) {
            cout << "# count HC: " << c << "; dt:  " << dt << endl;
        });

    work_point_t result_ts = tabu_search(
        cost_func,
        [&]() { return start_point; },
        generate_tsp_point_neighbours,
        iterations,
        100,
        [](int c, double dt) {
            cout << "# count TS: " << c << "; dt:  " << dt << endl;
        });


    //    cout << "best cost for: " << make_pair(problem, best_solution) << "is " << cost_func(best_solution) << endl;
    // cout << "best BF:\n"
    //      << make_pair(cities_coordinates, best_solution) << " is " << cost_func(best_solution) << endl;
    cout << "best HC:\n"
         << make_pair(cities_coordinates, result_hc) << " is " << cost_func(result_hc) << endl;
    cout << "best TS:\n"
         << make_pair(cities_coordinates, result_ts) << " is " << cost_func(result_ts) << endl;

    return 0;
}