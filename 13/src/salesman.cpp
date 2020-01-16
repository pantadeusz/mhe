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
#include "geneticalgorithm.hpp"
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
#include <mutex>

/*
 The things that are required for Genetic Algorithm
 */

auto standard_deviation = [](auto const &func) -> double {
  using namespace std;
  double mean = accumulate(func.begin(), func.end(), 0.0) / func.size();
  double sq_sum =
      inner_product(func.begin(), func.end(), func.begin(), 0.0, std::plus<>(),
                    [mean](double const &x, double const &y) {
                      return (x - mean) * (y - mean);
                    });
  return sqrt(sq_sum / (func.size() - 1));
};

/**
 * @brief standard interface for crossover
 *
 * @tparam specimen_t
 */
template <class specimen_t> class crossover_i {
public:
  double crossover_probability;
  crossover_i() { crossover_probability = 0.0; };
  crossover_i(double p) { crossover_probability = p; };

  virtual std::pair<specimen_t, specimen_t> crossover(specimen_t &a,
                                                      specimen_t &b) = 0;

  std::pair<specimen_t, specimen_t> operator()(specimen_t &a, specimen_t &b) {
    using namespace std;
    double u = uniform_real_distribution<double>(0.0, 1.0)(generator);
    if (u < crossover_probability) {
      return crossover(a, b);
    } else {
      return pair<specimen_t, specimen_t>(a, b);
    }
  };
};

template <class specimen_t>
class crossover_one_point_t : public crossover_i<specimen_t> {
public:
  crossover_one_point_t(double p) : crossover_i<specimen_t>(p){};

  std::pair<specimen_t, specimen_t> crossover(specimen_t &a, specimen_t &b) {
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
};

template <class specimen_t>
class crossover_two_point_t : public crossover_i<specimen_t> {
public:
  crossover_two_point_t(double p) : crossover_i<specimen_t>(p){};

  std::pair<specimen_t, specimen_t> crossover(specimen_t &a, specimen_t &b) {

    using namespace std;
    int cross_point_a =
        uniform_int_distribution<int>(0, a.solution.size() - 1)(generator);
    int cross_point_b =
        uniform_int_distribution<int>(0, a.solution.size() - 1)(generator);
    if (cross_point_a > cross_point_b)
      swap(cross_point_a, cross_point_b);
    auto new_a = a;
    auto new_b = b;

    for (int i = cross_point_a; i < cross_point_b; i++) {
      new_a.solution[i] = b.solution[i];
      new_b.solution[i] = a.solution[i];
    }
    return pair<alternative_solution_t, alternative_solution_t>(new_a, new_b);
  };
};

/**
 * @brief The common interface for mutation
 *
 * @tparam specimen_t the type of the specimen
 */
template <class specimen_t> class mutation_i {
public:
  mutation_i(){};
  virtual specimen_t operator()(specimen_t &a) = 0;
};

/**
 * @brief implementation of the mutation for the specimen that represents
 * salesman travel
 *
 * @tparam specimen_t the specimen must be that the number on i-th place is
 * from the range [0,i-1]
 */
template <class specimen_t>
class mutation_change_one_city_descending_c : public mutation_i<specimen_t> {
public:
  double mutation_probability;
  mutation_change_one_city_descending_c(double mutation_probability_)
      : mutation_probability(mutation_probability_){};
  virtual specimen_t operator()(specimen_t &a) {
    using namespace std;
    double u = uniform_real_distribution<double>(0.0, 1.0)(generator);
    if (u < mutation_probability) {
      int mut_point =
          uniform_int_distribution<int>(0, a.solution.size() - 2)(generator);
      specimen_t new_a = a;
      new_a.solution[mut_point] = uniform_int_distribution<int>(
          0, (a.solution.size() - 1) - mut_point)(generator);
      return new_a;
    } else {
      return a;
    }
  };
};

template <class specimen_t>
std::shared_ptr<mutation_i<specimen_t>>
mutation_factory(std::string /*mutation_name*/, double mutation_probability) {
  using namespace std;
  shared_ptr<mutation_i<specimen_t>> ret =
      make_shared<mutation_change_one_city_descending_c<specimen_t>>(
          mutation_probability);
  return ret;
};

template <class specimen_t>
std::shared_ptr<crossover_i<specimen_t>>
crossover_factory(std::string crossover_name, double crossover_probability) {
  using namespace std;

  if (crossover_name == "crossover_one_point") {
    return make_shared<crossover_one_point_t<specimen_t>>(
        crossover_probability);
  } else if (crossover_name == "crossover_two_point") {
    return make_shared<crossover_two_point_t<specimen_t>>(
        crossover_probability);
  } else {
    std::cerr << "[WW] falling back to default crossover: crossover_one_point"
              << std::endl;
    return make_shared<crossover_one_point_t<specimen_t>>(
        crossover_probability);
  }
};

auto term_condition_factory = [](auto args, auto example_solution) -> std::function<bool(std::vector<decltype(example_solution)> &, std::vector<double> &, int)>{
  using namespace std;
  /// for debugging purposes - we can print the population if the parameter
  /// print_population_stats is set to true
  auto print_population = (args["print_population_stats"] == "true")?[](vector<decltype(example_solution)> &pop,
                             vector<double> &fit, int iteration) {
    cout << "[print_population_stats] " << iteration << 
    " " << *max_element(fit.begin(), fit.end()) << 
    " " << (accumulate(fit.begin(), fit.end(), 0.0) / (double)fit.size()) << 
    " " << standard_deviation(fit) << " ";   
    for (unsigned i = 0; i < pop.size(); i++) {
      cout << " " << (pop[i].goal() / 1000.0)<<":" << fit[i];
    }
    cout << endl;
  }:[](vector<decltype(example_solution)> &,
                          vector<double> &, int){};
  /// default iteration count is 10. We can set it
  int iteration_count =
      args.count("iteration_count") ? stoi(args["iteration_count"]) : 10;
  if (args.count("iteration_count")) {
    return [iteration_count,
            print_population](vector<decltype(example_solution)> &pop,
                              vector<double> &fit, int iteration) {
      print_population(pop, fit, iteration);
      return iteration < iteration_count;
    };
  } else {
    auto tc = [print_population](vector<decltype(example_solution)> &pop,
                                 vector<double> &fit, int iteration) {
      print_population(pop, fit, iteration);
      if (standard_deviation(fit) < 0.01) {
        std::cerr << "[stddev] " << standard_deviation(fit) << " at "  << iteration << std::endl;
        return false;
      }
      return iteration < 1000;
    };
    return tc;
  }
};

auto tournament_selection = [](std::vector<double> &fitnesses,
                               int /*iteration*/) -> int {
  int first =
      std::uniform_int_distribution<int>(0, fitnesses.size() - 1)(generator);

  int second =
      std::uniform_int_distribution<int>(0, fitnesses.size() - 1)(generator);
  return (fitnesses[first] > fitnesses[second]) ? first : second;
};

/**
 * roulette selection
 * */
// (option) std::mutex roulette_selection_mutex;
auto roulette_selection = [](std::vector<double> &fitnesses,
                             int iteration) -> int {
  using namespace std;
  static int prev_iteration = 0;
  static double sum_fit = accumulate(fitnesses.begin(), fitnesses.end(), 0.0);
  if (prev_iteration != iteration) { // only calculate summary fitness if the
                                     // iteration is changed
    prev_iteration = iteration;
    sum_fit =
        accumulate(fitnesses.begin(), fitnesses.end(), 0.0); // optimization
  }

  double u = 0.0;
  //(option) {
  //(option)   lock_guard<mutex> guard(roulette_selection_mutex);
    u = uniform_real_distribution<double>(0.0, sum_fit)(generator);
  //(option) }
  for (int i = (int)(fitnesses.size() - 1); i >= 0; i--) {
    sum_fit -= fitnesses[i];
    if (sum_fit <= u)
      return i;
  }
  return 0;
};

/**
 * selekcja rangowa
 * */
auto rank_selection = [](std::vector<double> &fitnesses, int iteration) -> int {
  using namespace std;

  vector<pair<double, int>> fitnesses_with_index;
  for (unsigned int i = 0; i < fitnesses.size(); i++) {
    fitnesses_with_index.push_back({fitnesses[i], i});
  }
  sort(fitnesses_with_index.begin(), fitnesses_with_index.end(),
       [](auto &a, auto &b) { return a.first < b.first; });
  std::vector<double> fit_new(fitnesses.size());
  for (unsigned int i = 0; i < fitnesses.size(); i++) {
    fit_new[i] = i + 1;
  }
  return fitnesses_with_index[roulette_selection(fit_new, iteration)].second;
};

auto selection_factory =
    [](std::string selection_name,
       auto /*args*/) -> std::function<int(std::vector<double> &fit, int)> {
  if (selection_name == "tournament_selection")
    return tournament_selection;
  else if (selection_name == "roulette_selection")
    return roulette_selection;
  else if (selection_name == "rank_selection")
    return rank_selection;
  return tournament_selection; ///< default
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
        args.count("population_size") ? stoi(args["population_size"]) : 500;
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
      return 10000000.0 / (1.0 + specimen.goal());
    };

    // selection function from fitnesses
    auto selection_f = selection_factory(
        args.count("selection") ? args["selection"] : "tournament_selection",
        args);

    // how probable is execution the crossover
    double crossover_probability = args.count("crossover_probability")
                                       ? stod(args["crossover_probability"])
                                       : 0.8;

    // how probable is executing the mutation
    double mutation_probability = args.count("mutation_probability")
                                      ? stod(args["mutation_probability"])
                                      : 0.1;

    // crossover function from
    auto crossover_f = crossover_factory<alternative_solution_t>(
        args.count("crossover") ? args["crossover"] : "crossover_two_point",
        crossover_probability);
    // mutation function working on the specimen
    auto mutation_f = mutation_factory<decltype(initial_population.at(0))>(
        "mutation_change_one_city_descending", mutation_probability);

    // what is the termination conditino. True means continue; false means
    // stop and finish
    auto term_condition_f =
        term_condition_factory(args, initial_population.at(0));

    return genetic_algorithm(initial_population, fitness_f, selection_f,
                             *crossover_f, *mutation_f, term_condition_f)
        .get_solution();
  };
methods["island_model_ga"] = [](auto problem, auto args) -> solution_t {
    // the size of population
    int population_size =
        args.count("population_size") ? stoi(args["population_size"]) : 500;
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
      return 10000000.0 / (1.0 + specimen.goal());
    };

    // selection function from fitnesses
    auto selection_f = selection_factory(
        args.count("selection") ? args["selection"] : "tournament_selection",
        args);

    // how probable is execution the crossover
    double crossover_probability = args.count("crossover_probability")
                                       ? stod(args["crossover_probability"])
                                       : 0.8;

    // how probable is executing the mutation
    double mutation_probability = args.count("mutation_probability")
                                      ? stod(args["mutation_probability"])
                                      : 0.1;

    // crossover function from
    auto crossover_f = crossover_factory<alternative_solution_t>(
        args.count("crossover") ? args["crossover"] : "crossover_two_point",
        crossover_probability);
    // mutation function working on the specimen
    auto mutation_f = mutation_factory<decltype(initial_population.at(0))>(
        "mutation_change_one_city_descending", mutation_probability);

    // what is the termination conditino. True means continue; false means
    // stop and finish
    auto term_condition_f =
        term_condition_factory(args, initial_population.at(0));

    return island_model_ga(initial_population, fitness_f, selection_f,
                             *crossover_f, *mutation_f, 100, 5)
        .get_solution();
  };
  //island_model_ga

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
