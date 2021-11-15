

// different methods
#include "m_bruteforce.hpp"
#include "m_hillclimb.hpp"
#include "m_tabu.hpp"

// problem definition
#include "tsp_problem.hpp"


#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include <map>

using namespace std; ///< bad practice, but useful in examples for MHE

function<work_point_t(map<string, string>)> generate_experiment_f(map<string, string> parameters)
{
}

map<string, string> args_to_map(vector<string> arguments)
{
    map<string, string> ret;
    string argname = "";
    for (auto param : arguments) {
        if ((param.size() > 2) && (param.substr(0, 2) == "--")) {
            argname = param.substr(2);
        } else {
            ret[argname] = param;
        }
    }
    return ret;
}

// the experiment
int main(int argc, char** argv)
{
    map<string, string> parameters = {
        {"size", "10"},
        {"iterations","1000"},
        {"method", "brute_force"},
        {"print_result","false"}};
    for (auto [k, v] : args_to_map(vector<string>(argv, argv + argc))) {
        parameters[k] = v; // overwrite default parameters with the ones from CLI
    }
    vector<pair<double, double>> cities_coordinates;
    auto problem = generate_random_problem(stoi(parameters["size"]),
        [&cities_coordinates](auto coords) { cities_coordinates = coords; });
    cout << problem << endl;
    auto cost_func = cost_function_factory(problem);

    int iterations = stoi(parameters["iterations"]);
    work_point_t best_solution;
    auto on_finish =
        [](int c, double dt) {
            cout << "# count: " << c << "; dt:  " << dt << endl;
        };
    auto on_step = [&](int i, double current_goal_val, double goal_v) {
        cout << i << " " << current_goal_val << " " << goal_v << endl;
    };
    if (parameters["method"] == "brute_force") {
        best_solution = brute_force_tsp(
            cost_func,
            [&]() { return generate_first_tsp_point(problem); },
            get_next_point, on_finish, on_step);
    } else if (parameters["method"] == "hill_climb_rnd") {
        auto start_point = generate_random_tsp_point(problem);
        best_solution = hill_climb_rnd(
            cost_func,
            [&]() { return start_point; },
            generate_random_tsp_neighbour,
            iterations, on_finish, on_step);
    } else if (parameters["method"] == "tabu_search") {
        auto start_point = generate_random_tsp_point(problem);
        best_solution = tabu_search(
            cost_func,
            [&]() { return start_point; },
            generate_tsp_point_neighbours,
            iterations,
            100, on_finish, on_step);
    }

    if (parameters["print_result"] == "true") {
        cout << "# best " << parameters["method"] << " solution:\n"
         << make_pair(cities_coordinates, best_solution) << endl;
    }
    cout << "# best " << parameters["method"] << " result: " << 
    cost_func(best_solution) << endl;

    return 0;
}