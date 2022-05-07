#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <any>
#include <chrono>
#include <list>
#include <set>


#include "global_random.hpp"

#include "simulated_annealing.hpp"
#include "full_revision.hpp"
#include "tabu.hpp"
#include "genetic_algorithm.hpp"


#include "tsp_problem.hpp"
#include "global_random.hpp"

auto arg = [](int argc, char** argv, std::string name, auto default_value) -> decltype(default_value) {
    using namespace std;
    string paramname = "";
    any ret = default_value;
    for (auto argument: vector<string>(argv, argv + argc)) {
        if ((argument.size() > 0) && (argument[0] == '-')) {
            if (paramname != "") {
                if (name == argument.substr(1)) ret = true;
            }
            paramname = argument.substr(1);
        } else if (name == paramname) {
            if (std::is_same_v<decltype(default_value), int>) ret = stoi(argument);
            else if (std::is_same_v<decltype(default_value), double>) ret = stod( argument );
            else if (std::is_same_v<decltype(default_value), char>) ret = argument.at(0);
            else if (std::is_same_v<decltype(default_value), bool>) ret = (argument=="true") || (argument=="1") || (argument=="yes");
            else ret = argument;
            paramname = "";
        }
    }
    return std::any_cast<decltype(default_value)>(ret);
};



void show_help() {
#ifndef __OPTIMIZE__
#define OPTIMIZED "optimized "
#else
#define OPTIMIZED ""
#endif
    std::cout << "# " << OPTIMIZED << "binary with date: " << __TIME__ << " " << __DATE__ << std::endl;
    std::cout << "-fname filename" << std::endl;
    std::cout << "-show_progress true/false" << std::endl;
}

std::pair<solution_t,std::chrono::duration<double>> experiment_full_revision(problem_t problem, bool show_progress) {
    using namespace std;
/// generate initial solution
    solution_t solution;
    for (int i = 0; i < problem.size(); i++) {
        solution.push_back(i);
    }

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> goal = [problem](auto s) {
        return tsp_problem_cost_function(problem, s);
    };
/// generate next solution for the method
    function<solution_t(solution_t)> next_solution =
    [](auto s) {
        next_permutation(s.begin(), s.end());
        return s;
    };
/// what is the termination condition
    function<bool(solution_t)> term_condition = [solution](auto s) {
        return !(s == solution);
    };
/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_full_review<solution_t>(solution, goal,next_solution, term_condition, show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    return {best_solution, calculation_duration};
}

solution_t generate_random_solution(problem_t problem) {
    solution_t solution;
    for (int i = 0; i < problem.size(); i++) {
        solution.push_back(i);
    }

    std::shuffle(solution.begin(), solution.end(), random_generator);

    return solution;
}


std::pair<solution_t,std::chrono::duration<double>> experiment_random_probe(problem_t problem,
int iterations, bool show_progress) {
    using namespace std;
/// generate initial solution
    solution_t solution = generate_random_solution(problem);

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> goal = [problem](auto s) {
        return tsp_problem_cost_function(problem, s);
    };
/// generate next solution for the method
    function<solution_t(solution_t)> next_solution = [problem](auto s) {
        return generate_random_solution(problem);
    };
/// what is the termination condition
    int i = 0;
    function<bool(solution_t)> term_condition = [&](auto s) {
        i++;
        return i < iterations;
    };
/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_full_review<solution_t>(solution,
                               goal,next_solution,
                               term_condition, show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    return {best_solution, calculation_duration};
}

std::pair<solution_t,std::chrono::duration<double>> experiment_random_hillclimb(problem_t problem,
int iterations, bool show_progress) {
    using namespace std;
/// generate initial solution
    solution_t solution = generate_random_solution(problem);

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> goal = [problem](auto s) {
        return tsp_problem_cost_function(problem, s);
    };
/// generate next solution for the method
    function<solution_t(solution_t)> next_solution = [problem](auto s) {
        uniform_int_distribution<int> distr(0,problem.size()-1);
        int a = distr(random_generator);
        int b = distr(random_generator);
        swap(s[a], s[b]);
        return s;
    };
/// what is the termination condition
    int i = 0;
    function<bool(solution_t)> term_condition = [&](auto s) {
        i++;
        return i < iterations;
    };
/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_full_review<solution_t>(solution,
                               goal,next_solution,
                               term_condition, show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    return {best_solution, calculation_duration};
}

std::pair<solution_t,std::chrono::duration<double>> experiment_tabu(problem_t problem,
int iterations, int max_tabu_size, bool show_progress) {
    using namespace std;
/// generate initial solution
    solution_t solution = generate_random_solution(problem);

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> goal = [problem](auto s) {
        return tsp_problem_cost_function(problem, s);
    };
/// generate next solution for the method
    function<vector<solution_t>(solution_t)> get_neighbors = [problem](auto s) -> vector<solution_t> {
        vector<solution_t> n;
        for (int i = 0; i < s.size(); i++) {
            auto tmp = s;
            swap(tmp[i], tmp[(i+1) % s.size()]);
            n.push_back(tmp);
        }
        return n;
    };
/// what is the termination condition
    int i = 0;
    function<bool(solution_t)> term_condition = [&](auto s) {
        i++;
        return i < iterations;
    };
/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_tabu<solution_t>(solution,
                               goal,get_neighbors,
                               term_condition, max_tabu_size, show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    return {best_solution, calculation_duration};
}


std::pair<solution_t,std::chrono::duration<double>> experiment_simulated_annealing(problem_t problem,
        int iterations,
bool show_progress) {
    using namespace std;
/// generate initial solution
    solution_t solution = generate_random_solution(problem);

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> goal = [problem](auto s) {
        return tsp_problem_cost_function(problem, s);
    };
/// generate next solution for the method
    function<solution_t(solution_t)> next_solution = [problem](auto s) {
        uniform_int_distribution<int> distr(0,problem.size()-1);
        int a = distr(random_generator);
        int b = distr(random_generator);
        swap(s[a], s[b]);
        return s;
    };
/// what is the termination condition
    int i = 0;
    function<bool(solution_t)> term_condition = [&](auto s) {
        i++;
        return i < iterations;
    };

    function<double(int)> temperature = [](int t) {
        return 1000/t;
    };
/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_simulated_annealing<solution_t>(solution,
                               goal,next_solution,
                               temperature,
                               term_condition,
                               show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    return {best_solution, calculation_duration};
}


std::pair<solution_t,std::chrono::duration<double>> experiment_genetic_algorithm(problem_t problem,
        int iterations,
        int population_size,
        double crossover_probability,
        double mutation_probability,
bool show_progress) {
    using namespace std;
/// generate initial solution
    vector<solution_t> init_population;
    for (int i = 0; i < population_size; i++)
        init_population.push_back( generate_random_solution(problem));

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> fitness_function = [problem](auto s) {
        map<int,int> stats;
        double error = 0;
        for (auto i : s) {
            stats[i] = stats[i] + 1;
        }
        for (auto [k,v] : stats) {
            if (v > 1) error += v;
        }
        double fit = 10000.0 + 1000.0/(1+tsp_problem_cost_function(problem, s)) - error*100;
        return (fit>0)?fit:0.0;
    };
/// mutation method
    function<solution_t(solution_t)> mutation = [=](auto s) {
        uniform_real_distribution<double> u(0.0,1.0);
        if (u(random_generator) < mutation_probability) {
            uniform_int_distribution<int> distr(0,problem.size()-1);
            int a = distr(random_generator);
            int b = distr(random_generator);
            swap(s[a], s[b]);
        }
        return s;
    };
    auto crossover = [=](solution_t a, solution_t b) -> pair<solution_t,solution_t> {
        uniform_real_distribution<double> u(0.0,1.0);
        if (u(random_generator) < crossover_probability) {
            uniform_int_distribution<int> cross_distr(0,a.size()-1);
            int crossover_pt = cross_distr(random_generator);
            for (int i = crossover_pt; i < a.size(); i++) swap(a[i], b[i]);
        }
        return {a,b};
    };
/// what is the termination condition
    int i = 0;
    std::function<bool(std::vector<double>, std::vector<solution_t>)>
    term_condition = [&](auto pop_fit,auto pop) {
        i++;
        return i < iterations;
    };

/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_genetic_algorithm<solution_t>(init_population,
                               fitness_function,
                               selection_tournament,
                               crossover,
                               mutation,
                               term_condition,
                               show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    return {best_solution, calculation_duration};
}




int main(int argc, char** argv)
{
    using namespace std;

/// get arguments
    auto fname = arg(argc, argv, "fname", std::string(""));
    auto show_progress = arg(argc, argv, "show_progress", false);
    auto iterations = arg(argc, argv, "iterations", 1000);
    auto method = arg(argc, argv, "method", std::string("tabu"));
    auto tabu_size = arg(argc, argv, "tabu_size", 100);
    auto dot =  arg(argc, argv, "dot", false);

    auto pop_size = arg(argc, argv, "pop_size", 50);
    auto crossover_p = arg(argc, argv, "crossover_p", 0.9);
    auto mutation_p = arg(argc, argv, "mutation_p", 0.1);

/// do we need to show some help?
    if (arg(argc, argv, "h", false)) {
        show_help();
        return 0;
    }
    cout << "# fname = " << fname << ";" << std::endl;

/// load the problem at hand
    auto problem = load_problem(fname);
    solution_t best_solution;
    std::chrono::duration<double> calculation_duration;
    if (method == "full_revision") {
        auto [a, b] = experiment_full_revision(problem, show_progress);
        best_solution = a;
        calculation_duration = b;
    } else     if (method == "random_probe") {
        auto [a, b] = experiment_random_probe(problem, iterations, show_progress);
        best_solution = a;
        calculation_duration = b;
    } else     if (method == "random_hillclimb") {
        auto [a, b] = experiment_random_hillclimb(problem, iterations, show_progress);
        best_solution = a;
        calculation_duration = b;
    } else     if (method == "tabu") {
        auto [a, b] = experiment_tabu(problem, iterations, tabu_size, show_progress);
        best_solution = a;
        calculation_duration = b;
    } else     if (method == "simulated_annealing") {
        auto [a, b] = experiment_simulated_annealing(problem, iterations, show_progress);
        best_solution = a;
        calculation_duration = b;


        // std::pair<solution_t,std::chrono::duration<double>> experiment_genetic_algorithm(problem_t problem,

    } else if (method == "genetic_algorithm") {
        auto [a, b] = experiment_genetic_algorithm(problem, iterations,
                      pop_size,
                      crossover_p,
                      mutation_p,
                      show_progress);
        best_solution = a;
        calculation_duration = b;
    } else {
        std::cerr << "unknown method" << std::endl;
    }
    cout << "# " << method << ": best_cost: " << tsp_problem_cost_function(problem, best_solution) << " calculation_time: " << calculation_duration.count() << endl;
    cout << "# solution: " << best_solution << endl;
    if (dot) cout << vis(best_solution, problem);
    return 0;
}
