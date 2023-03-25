

// different methods
#include "m_bruteforce.hpp"
#include "m_ga.hpp"
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
        {"iterations", "1000"},
        {"method", "brute_force"},
        {"print_result", "false"},
        {"tabu_size", "100"},
        {"p_mutation", "0.1"},
        {"p_crossover", "0.9"},
        {"pop_size", "100"}};
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
       // cout << i << " " << current_goal_val << " " << goal_v << endl;
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
            stoi(parameters["tabu_size"]), on_finish, on_step);
    } else if (parameters["method"] == "genetic_algorithm") {
        random_device dandom_dev;
        mt19937 rand_gen(dandom_dev());

        int n = stof(parameters["pop_size"]);
        vector<work_point_t> population(n);
        generate(population.begin(), population.end(), [&problem]() { return generate_random_tsp_point(problem); });
        auto fitness = [&](work_point_t specimen) { return std::max(0.0, 10000.0 - cost_func(specimen)); };
        auto term_condition = [&](auto population, auto population_fit, auto iteration) {
            return iteration < iterations;
        };
        double p_crossover = stof(parameters["p_crossover"]);
        double p_mutation = stof(parameters["p_mutation"]);
        auto selection = [&](auto population_fit) {
            uniform_int_distribution<int> distr(0, population_fit.size() - 1);
            int a = distr(rand_gen), b = distr(rand_gen);
            return (population_fit[a] > population_fit[b]) ? a : b;
        };
        auto crossover_pmx = [&](auto parent0, auto parent1) {
            if (parent0.size() != parent1.size()) throw std::invalid_argument("both specimens should have the same size");
            uniform_int_distribution<int> distr(1, parent0.size() - 2);
            int a = distr(rand_gen), b = distr(rand_gen); // punkty podzialu
            if (a > b) swap(a, b);
            if (a == b) return make_pair(parent0, parent1);
            auto child0 = parent1;
            auto child1 = parent0;
            auto is_already_present = [&](int city, auto& child) {
                for (int i = a; i < b; i++) {
                    if (city == child[i]) return true;
                }
                return false;
            };
            auto get_mapping = [&](int city, auto p0, auto p1) {
                for (int i = a; i < b; i++) {
                    if (city == p1[i]) return p0[i];
                }
                throw std::invalid_argument("invalid mapping for crossover");
            };
            for (int i = 0; i < parent0.size(); i = ((i + 1) == a) ? b : (i + 1)) {
                child0[i] = parent0[i];
                while (is_already_present(child0[i], child0)) {
                    child0[i] = get_mapping(child0[i], parent0, parent1);
                }
                child1[i] = parent1[i];
                while (is_already_present(child1[i], child1)) {
                    child1[i] = get_mapping(child1[i], parent1, parent0);
                }
            }
            return make_pair(child0, child1);
        };
        auto mutation = [&](auto specimen) {
            uniform_int_distribution<int> distr(0, specimen.size() - 1);
            int a = distr(rand_gen);
            int b = distr(rand_gen);
            std::swap(specimen[a], specimen[b]);
            return specimen;
        };
        auto result_population = genetic_algorithm(fitness, population, term_condition, selection, crossover_pmx, p_crossover, mutation, p_mutation, on_finish, on_step);
        best_solution = result_population[0];
    }

    if (parameters["print_result"] == "true") {
        cout << "# best " << parameters["method"] << " solution:\n"
             << make_pair(cities_coordinates, best_solution) << endl;
    }
    cout << "# best " << parameters["method"] << " result: " << cost_func(best_solution) << endl;

    return 0;
}