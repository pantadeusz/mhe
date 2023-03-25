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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "json.hpp"

#include "salesman.hpp"
#include "salesman_html_skel.cpp"


/**
 * We need some randomness. First random device (hopefully hardware)
 * */
std::random_device seedsource;
/**
 * Then create default random engine
 * */
std::default_random_engine generator(seedsource());



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

solution_t brute_force_find_solution(std::shared_ptr<problem_t> problem_) {
  using namespace std;
  solution_t current(problem_);  // current solution
  solution_t best(problem_);     // best solution so far
  solution_t problem0(problem_); // first solution

  do {
    current = current.next_solution();
    if (best.goal() > current.goal())
      best = current;
  } while (!(current == problem0));

  return best;
}

/**
 * Hill climbing algorithm - randomized version
 * */
solution_t hillclimb(std::shared_ptr<problem_t> problem,
                     const int iterations = 1000) {
  using namespace std;
  auto solution = alternative_solution_t::of(problem, generator);
  // there must be more cities than 1
  for (int iteration = 0; iteration < iterations; iteration++) {
    auto new_solution = solution.generate_random_neighbour(1, generator);
    if (new_solution.get_solution().goal() < solution.get_solution().goal()) {
      solution = new_solution;
    }
  };

  return solution.get_solution();
}

/**
 * TODO: Next lecture
 * 
 * hill climb - deterministic version
 * tabu search
 */

/*
solution_t tabusearch(std::shared_ptr<problem_t> problem,const int iterations =
1000, const int max_tabu_size = 1000) { using namespace std;
  list<alternative_solution_t> tabu_list; // tabu
  tabu_list.push_back(alternative_solution_t::of(problem, generator)); // start
point
  // repeat
  for (int iteration = 0; iteration < iterations; iteration++) {
    tabu_list.push_back(tabu_list.back().generate_random_neighbour(1,
generator));
  }
  // get best element from tabu and best found so far - will change on lecture
  return min_element(tabu_list.begin(), tabu_list.end(),
                     [](auto a, auto b) {
                       return a.get_solution().goal() < b.get_solution().goal();
                     })
      ->get_solution();
}
*/

/**
 * @brief Main experiment
 *
 * You can provide input in json format. It can be as standard input and then
 * the result would be standard output, or you can give arguments:
 * ./app input.json output.json
 *
 */
int main(int argc, char **argv) {
  using namespace std;

  solution_t experiment;
  if (argc > 2) {
    std::ifstream is(argv[1]); // open file
    is >> experiment;
  } else {
    cin >> experiment;
  }

  auto start_time_moment = chrono::system_clock::now();
  // experiment_result = brute_force_find_solution(experiment.problem);
  auto experiment_result = hillclimb(experiment.problem);
  // experiment_result = tabusearch(experiment.problem);
  auto end_time_moment = std::chrono::system_clock::now();
  chrono::duration<double> time_duration = end_time_moment - start_time_moment;
  cerr << "[I] calculation_time: " << time_duration.count() << endl;
  cerr << "[I] solution_goal_value: " << experiment_result.goal() / 1000.0
       << endl;

  if (argc > 2) {
    std::ofstream os(argv[2]); // open file
    os << experiment_result;
  } else {
    cout << experiment_result << endl;
  }

  // save visualization on map
  std::ofstream htmlout("vis.html");
  htmlout << html_header;
  htmlout << experiment_result;
  htmlout << html_footer;
  htmlout.close();
}
