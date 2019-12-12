/**
 * @file salesman.cpp
 * @author Tadeusz Puźniakowski (pantadeusz@pjwstk.edu.pl)
 * @brief The example of TSP problem and solution
 * @version 0.1
 * @date 2019-10-15
 *
 * @copyright Copyright (c) 2019
 *
 *
 * Compilation:
 * g++ -std=c++17 salesman.cpp -o salesman -O3
 * Run:
 * ./salesman ./input.json wyniki.json
 */

#include "helper.hpp"

#include "brute.hpp"
#include "hillclimb.hpp"
#include "simulatedannealing.hpp"
#include "tabu.hpp"

#include "salesman.hpp"
#include "salesman_html_skel.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

auto genetic_algorithm = [](auto initial_population, auto fitness_f,
                            auto selection_f, auto crossover_f, auto mutation_f,
                            auto term_condition_f) {
  using namespace std;

  auto population = initial_population;
  int iteration = 0;
  while (term_condition_f(population, iteration++)) {
    vector<double> fit;                    ///< list of fitnesses
    decltype(initial_population) parents;  ///< parents selected
    decltype(initial_population) children; ///< offspring
    for (auto &specimen : population)
      fit.push_back(fitness_f(specimen)); ///< calculate fitnesses
    /// select speciments
    for (unsigned int i = 0; i < initial_population.size(); i++) {
      parents.push_back(population[selection_f(fit)]);
    }
    /// do the crossover
    for (int i = 0; i < (int)initial_population.size() - 1; i += 2) {
      auto [a, b] = crossover_f(parents[i], parents[i + 1]);
      children.push_back(a);
      children.push_back(b);
    }

    /// do the mutation
    for (int i = 0; i < (int)initial_population.size() - 1; i += 2) {
      children[i] = mutation_f(children[i]);
    }
    population = children;
  }
  /// return the best specimen of all
  return *std::max_element(
      population.begin(), population.end(),
      [&](auto &l, auto &r) { return fitness_f(l) < fitness_f(r); });
};

auto crossover_one_point = [](auto &a, auto &b) {
  using namespace std;
  int cross_point =
      uniform_int_distribution<int>(0, a.solution.size() - 1)(generator);
  auto new_a = a;
  auto new_b = b;

  for (int i = cross_point; i < (int)a.solution.size(); i++) {
    new_a.solution[i] = b.solution[i];
    new_b.solution[i] = a.solution[i];
  }
  return pair<alternative_solution_t, alternative_solution_t>(new_a, new_b);
};

/**
 * the mutation that flips one random bit/gene
 * */
auto mutation_one_bit_flip = [](auto &a,
                                double mutation_probability) -> decltype(a) {
  using namespace std;
  double u = uniform_real_distribution<double>(0.0, 1.0)(generator);
  if (mutation_probability < u) {
    int mut_point =
        uniform_int_distribution<int>(0, a.solution.size() - 2)(generator);
    decltype(a) new_a = a;
    new_a.solution[mut_point] = uniform_int_distribution<int>(
        0, (a.solution.size() - 1) - mut_point)(generator);
    return new_a;
  } else {
    return a;
  }
};

auto mutation_factory = [](std::string mutation_name,
                           double mutation_probability, auto example_solution)
    -> std::function<decltype(example_solution)(decltype(example_solution) &)> {
  auto default_mutation =
      [mutation_probability](
          decltype(example_solution) &a) -> decltype(example_solution) {
    return mutation_one_bit_flip(a, mutation_probability);
  };
  if (mutation_name == "mutation_one_bit_flip") {
    return default_mutation;
  } else {
    std::cerr << "[WW] falling back to default mutation: mutation_one_bit_flip"
              << std::endl;
    return default_mutation;
  }
};

auto crossover_factory = [](std::string crossover_name,
                            double crossover_probability,
                            auto example_solution) {
  using namespace std;
  auto default_crossover = [crossover_probability](
                               decltype(example_solution) &a,
                               decltype(example_solution) &b) {
    double u = uniform_real_distribution<double>(0.0, 1.0)(generator);
    if (crossover_probability < u) {
      return crossover_one_point(a, b);
    } else {
      return pair<decltype(example_solution), decltype(example_solution)>(a, b);
    }
  };
  if (crossover_name == "crossover_one_point") {
    return default_crossover;
  } else {
    std::cerr << "[WW] falling back to default crossover: crossover_one_point"
              << std::endl;
    return default_crossover;
  }
};

auto term_condition_factory = [](auto args) {
  using namespace std;
  int iteration_count =
      args.count("iteration_count") ? stoi(args["iteration_count"]) : 10;
  auto default_tc = [iteration_count](std::vector<alternative_solution_t> &pop,
                                      int iteration) {
    //for (auto &s : pop) {
    //  cout << (s.goal() / 1000.0) << " ";
    //}
    //cout << endl;

    return iteration < iteration_count;
  };
  return default_tc;
};

using method_f = std::function<solution_t(std::shared_ptr<problem_t>,
                                          std::map<std::string, std::string>)>;

std::map<std::string, method_f> generate_methods_map() {
  using namespace std;

  map<string, method_f> methods;

  methods["brute_force_find_solution"] = [](auto problem, auto /*args*/) {
    solution_t current(problem); // current solution
    return brute_force_find_solution<solution_t>(current);
  };
  methods["hillclimb"] = [](auto problem, auto /*args*/) {
    auto solution = alternative_solution_t::of(problem, generator);
    return hillclimb<alternative_solution_t, std::default_random_engine>(
               solution, generator, 1000)
        .get_solution();
  };
  methods["hillclimb_deteriministic"] = [](auto problem, auto /*args*/) {
    auto solution = alternative_solution_t::of(problem, generator);
    return hillclimb_deteriministic(solution).get_solution();
  };
  methods["tabusearch"] = [](auto problem, auto /*args*/) {
    auto problem0 = alternative_solution_t::of(problem, generator);
    return tabusearch<alternative_solution_t>(problem0).get_solution();
  };
  methods["simulated_annealing"] = [](auto problem, auto /*args*/) {
    auto p0 = alternative_solution_t::of(problem, generator);
    return simulated_annealing<alternative_solution_t,
                               std::default_random_engine>(p0, generator)
        .get_solution();
  };
  methods["genetic_algorithm"] = [](auto problem, auto args) -> solution_t {
    // the size of population
    int population_size =
        args.count("population_size") ? stoi(args["population_size"]) : 10;
    // initial population
    std::vector<alternative_solution_t> initial_population = [problem](int n) {
      std::vector<alternative_solution_t> pop;
      while (n--) {
        pop.push_back(alternative_solution_t::of(problem, generator));
      }
      return pop;
    }(population_size);
    // fitness function
    auto fitness_f = [](alternative_solution_t specimen) {
      return 1000.0 / (1.0 + specimen.goal());
    };

    // selection function from fitnesses
    auto selection_f = [](vector<double> &fitnesses) {
      int first = std::uniform_int_distribution<int>(0, fitnesses.size() -
                                                            1)(generator);

      int second = std::uniform_int_distribution<int>(0, fitnesses.size() -
                                                             1)(generator);
      return (fitnesses[first] > fitnesses[second]) ? first : second;
    };

    // how probable is execution the crossover
    double crossover_probability = args.count("crossover_probability")
                                       ? stod(args["crossover_probability"])
                                       : 0.9;
    // how probable is executing the mutation
    double mutation_probability = args.count("mutation_probability")
                                      ? stod(args["mutation_probability"])
                                      : 0.1;

    // crossover function from
    auto crossover_f = crossover_factory(
        "crossover_one_point", crossover_probability, initial_population.at(0));
    // mutation function working on the specimen
    auto mutation_f =
        mutation_factory("mutation_one_bit_flip", mutation_probability,
                         initial_population.at(0));
    // what is the termination conditino. True means continue; false means stop
    // and finish

    auto term_condition_f = term_condition_factory(args);

    return genetic_algorithm(initial_population, fitness_f, selection_f,
                             crossover_f, mutation_f, term_condition_f)
        .get_solution();
  };

  return methods;
}

/**
 * @brief Main experiment
 *
 * You can provide input in json format. It can be as standard input and then
 * the result would be standard output, or you can give arguments:
 * ./app input.json output.json
 *
 */
int main(int argc, char **argv_) {
  using namespace std;
  auto methods = generate_methods_map();

  solution_t experiment;

  auto arguments_map = process_arguments(argc, argv_);
  string selected_method_name = (arguments_map.count("method") > 0)
                                    ? arguments_map["method"]
                                    : "brute_force_find_solution";
  if (methods.count(selected_method_name) == 0) {
    for (auto &[name, f] : methods)
      cout << name << " ";
    cout << endl;
    throw invalid_argument("Please provide correct method name.");
  }
  if (arguments_map.count("in")) {
    std::ifstream is(arguments_map["in"]); // open file
    is >> experiment;
  } else {
    cin >> experiment;
  }

  auto start_time_moment = chrono::system_clock::now();

  auto experiment_result =
      methods[selected_method_name](experiment.problem, arguments_map);
  auto end_time_moment = std::chrono::system_clock::now();
  chrono::duration<double> time_duration = end_time_moment - start_time_moment;
  cerr << "[I] method_name: " << selected_method_name << endl;
  cerr << "[I] calculation_time: " << time_duration.count() << endl;
  cerr << "[I] solution_goal_value: " << experiment_result.goal() / 1000.0
       << endl;

  if (arguments_map.count("out")) {
    std::ofstream os(arguments_map["out"]); // open file
    os << experiment_result;
  } else {
    cout << experiment_result << endl;
  }

  // save visualization on map
  if (arguments_map.count("html")) {
    std::ofstream htmlout(arguments_map["html"]);
    htmlout << html_header;
    htmlout << experiment_result;
    htmlout << html_footer;
    htmlout.close();
  }
  return 0;
}
