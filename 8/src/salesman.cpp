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

#include "brute.hpp"
#include "hillclimb.hpp"

#include "salesman.hpp"
#include "salesman_html_skel.hpp"

#include "json.hpp"

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

/**
 * @brief prints solution to output stream
 *
 * overrides the << operator, so you can write ```cout << solution```
 */
inline std::ostream &operator<<(std::ostream &s, const solution_t &sol) {
  using namespace nlohmann;
  json j;
  j["cities"] = json::array();
  for (auto city : sol.cities_to_see) {
    j["cities"].push_back({sol.problem->cities[city].name,
                           sol.problem->cities[city].latitude,
                           sol.problem->cities[city].longitude});
  }
  j["goal"] = sol.goal() / 1000.0;
  s << j.dump(4);
  return s;
}

/**
 * @brief reads solution from input stream
 *
 * overrides the >> operator, so you can write ```cin >> solution```
 */
inline std::istream &operator>>(std::istream &s, solution_t &sol) {
  nlohmann::json sol_json;
  sol_json = sol_json.parse(s);
  std::vector<city_t> cities;
  for (auto element : sol_json["cities"]) {
    city_t c{element[0], element[1], element[2]};
    cities.push_back(c);
  }
  solution_t solnew(cities);
  sol = solnew;
  return s;
}

std::list<alternative_solution_t>
generate_neighbours(alternative_solution_t &tmp_sol_) {
  std::list<alternative_solution_t> ret;

  for (int i = 0; i < (int)tmp_sol_.solution.size() - 1; i++) {
    alternative_solution_t nsol = tmp_sol_;
    nsol.solution[i] = (nsol.solution[i] + 1) % (tmp_sol_.solution.size() - i);
    ret.push_back(nsol);
    nsol = tmp_sol_;
    nsol.solution[i] = (nsol.solution[i] - 1 + (tmp_sol_.solution.size() - i)) %
                       (tmp_sol_.solution.size() - i);
    ret.push_back(nsol);
  }
  return std::move(ret);
}

/**
 * Hill climbing algorithm - randomized version
 * */
solution_t hillclimb_deteriministic(std::shared_ptr<problem_t> problem,
                                    const int iterations = 1000) {
  using namespace std;
  auto solution = alternative_solution_t::of(problem, generator);
  // there must be more cities than 1
  for (int iteration = 0; iteration < iterations; iteration++) {
    auto neighbours = generate_neighbours(solution);
    auto current_best = neighbours.back();
    for (auto &sol : neighbours) {
      if (sol.get_solution().goal() < current_best.get_solution().goal()) {
        current_best = sol;
      }
    }
    if (current_best.get_solution().goal() < solution.get_solution().goal()) {
      solution = current_best;
    } else {
      cerr << "found solution earlier " << iteration << " (before "
           << iterations << ")" << endl;
      return solution.get_solution();
    }
  };

  return solution.get_solution();
}

/**
 * Simulated annealing algorithm to the traveling salesman problem.
 * */
solution_t simulated_annealing(std::shared_ptr<problem_t> problem,
                               const int iterations = 1000,
                               std::function<double(int)> T = [](int k) {
                                 return 4000000.0 / (double)k;
                               }) {
  using namespace std;
  list<alternative_solution_t> s;
  s.push_back(alternative_solution_t::of(problem, generator));
  // there must be more cities than 1
  for (int k = 1; k <= iterations; k++) {
    auto new_solution = s.back().generate_random_neighbour(1, generator);
    if (new_solution.get_solution().goal() < s.back().get_solution().goal()) {
      s.push_back(new_solution);
    } else {
      double u = uniform_real_distribution<double>(0.0, 1.0)(generator);
      auto f_t_k = new_solution.get_solution().goal();
      auto f_s_k_1 = s.back().get_solution().goal();
      if (u < exp(-abs(f_t_k - f_s_k_1) / T(k))) {
        s.push_back(new_solution);
      } else {
        s.push_back(s.back()); // for compatibility with pseudocode
      }
    }
  };

  // for (auto e: s) {
  //   // static int i = 0;
  // // i++;
  //   // cerr << i << " " << (e.get_solution().goal()/1000.0) << endl;
  // }
  return min_element(s.begin(), s.end(),
                     [](auto a, auto b) {
                       return (a.get_solution().goal() <
                               b.get_solution().goal());
                     })
      ->get_solution();
}

bool operator==(const alternative_solution_t &a,
                const alternative_solution_t &b) {
  for (unsigned i = 0; i < a.solution.size(); i++) {
    if (a.solution.at(i) != b.solution.at(i))
      return false;
  }
  return true;
}
/**
 * tabu search
 */
solution_t tabusearch(std::shared_ptr<problem_t> problem,
                      const int iterations = 1000,
                      const int max_tabu_size = 50) {
  using namespace std;
  list<alternative_solution_t> tabu_list; // tabu
  tabu_list.push_back(alternative_solution_t::of(problem, generator));
  alternative_solution_t global_best = tabu_list.back();
  // repeat
  for (int iteration = 0; iteration < iterations; iteration++) {
    auto neighbours = generate_neighbours(tabu_list.back());
    // delete elements that are in tabu
    neighbours.erase(std::remove_if(neighbours.begin(), neighbours.end(),
                                    [&tabu_list](const auto &te) {
                                      for (auto &e : tabu_list)
                                        if (e == te)
                                          return true;
                                      return false;
                                    }),
                     neighbours.end());

    if (neighbours.size() <= 0) {
      // impossible to find next solution
      if (tabu_list.size() > 1)
        tabu_list.pop_front();
      else
        return global_best.get_solution();
    } else {
      // find best neighbour
      auto current_best = neighbours.back();
      for (auto &sol : neighbours) {
        if (sol.get_solution().goal() < current_best.get_solution().goal()) {
          current_best = sol;
        }
      }
      // dodajemy lepszego
      tabu_list.push_back(current_best);
      if (current_best.get_solution().goal() <
          global_best.get_solution().goal()) {
        global_best = current_best;
      }

      // update global best
      if ((int)tabu_list.size() >= max_tabu_size)
        tabu_list.pop_front();
    }
  }
  return global_best.get_solution();
}

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
    return hillclimb<alternative_solution_t,std::default_random_engine>
    (solution, generator, 1000).get_solution();
  };
  methods["hillclimb_deteriministic"] = [](auto problem, auto /*args*/) {
    return hillclimb_deteriministic(problem);
  };
  methods["tabusearch"] = [](auto problem, auto /*args*/) {
    return tabusearch(problem);
  };
  methods["simulated_annealing"] = [](auto problem, auto /*args*/) {
    return simulated_annealing(problem);
  };

  return methods;
}

auto process_arguments = [](int argc, char **argv) {
  using namespace std;
  map<string, string> arguments_map;
  for (auto a : vector<string>(argv + 1, argv + argc)) {
    static string k = "";
    if (a.substr(0, 1) == "-") {
      k = string(a.begin() + 1, a.end());
    } else
      arguments_map[k] = a;
  }
  if (arguments_map.count("h") + arguments_map.count("H") +
      arguments_map.count("help")) {
    cout << "arguments:" << endl;
    cout << "in [input]" << endl;
    cout << "out [output]" << endl;
    cout << "html [html_visualization_file_name]" << endl;
    throw invalid_argument("help was needed");
  }
  return arguments_map;
};
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
