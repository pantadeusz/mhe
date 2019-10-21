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

std::random_device seedsource;
std::default_random_engine generator(seedsource());

solution_t hillclimb(std::shared_ptr<problem_t> problem) {
  using namespace std;
  auto solution = alternative_solution_t::of(problem, generator);
  // there must be more cities than 1
  for (int iteration = 0; iteration < 100000; iteration++) {
    alternative_solution_t solution_candidate =
        solution.generate_random_neighbour(1, generator);

    if (solution_candidate.get_solution().goal() <
        solution.get_solution().goal()) {
      cout << solution_candidate.get_solution().goal() / 1000.0 << "  vs  "
           << solution.get_solution().goal() / 1000.0 << endl;
      solution = solution_candidate;
    }
  };

  return solution.get_solution();
}

/*

TODO: Nastepny wyklad

solution_t tabusearch(std::shared_ptr<problem_t> problem) {
  using namespace std;
  list<alternative_solution_t> solution(1);

  // list containing one correct solution
  solution.back().solution.resize(problem->cities.size());
  for (int i = 0; i < solution.back().solution.size(); i++) {
    std::uniform_int_distribution<int> distribution(
        0, solution.back().solution.size() - 1 - i);
    solution.back().solution[i] = distribution(generator);
  }
  solution.back().problem = problem;

   // TODO - nastepny wyklad !!

  auto is_in_tabu = [&](auto sol) {
    for (auto &s: solution) {

    }
  };

  // there must be more cities than 1
  for (int iteration = 0; iteration < 100000; iteration++) {

    alternative_solution_t solution_candidate = solution;
    solution_candidate.solution[idx] = (solution_candidate.solution[idx]
        + (2*dist01(generator)-1)
        + solution.solution.size()-idx) %
          (solution.solution.size()-idx);

    if (solution_candidate.get_solution().goal() <
solution.get_solution().goal()) { solution = solution_candidate; cout <<
solution.get_solution().goal()/1000.0 << endl;
    }
  };

  return solution.get_solution();
}

*/

void test_alternative_solution_t() {
  solution_t solution0(
      {{"Januszowo", 1, 1}, {"Tortowo", 2, 2}, {"Wacikowo", 3, 3}});
  using namespace std;
  std::swap(solution0.cities_to_see[0], solution0.cities_to_see[1]);
  for (auto i : solution0.cities_to_see)
    cout << i << " ";
  cout << endl;
  auto solution = alternative_solution_t::of(solution0);
  for (auto i : solution.solution)
    cout << i << " ";
  cout << " <- alternative_solution_t solution.solution" << endl;
  for (auto i : solution.get_solution().cities_to_see)
    cout << i << " ";
  cout << endl;
  for (auto i : solution.set_solution(solution0).solution)
    cout << i << " ";
  cout << endl;
}

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
  // test_alternative_solution_t();
  // return 0;
  solution_t experiment;
  if (argc > 2) {
    std::ifstream is(argv[1]); // open file
    is >> experiment;
  } else {
    cin >> experiment;
  }

  // experiment = brute_force_find_solution(experiment.problem);
  experiment = hillclimb(experiment.problem);

  if (argc > 2) {
    std::ofstream os(argv[2]); // open file
    os << experiment;
  } else {
    cout << experiment << endl;
  }

  // save visualization on map
  std::ofstream htmlout("vis.html");
  htmlout << html_header;
  htmlout << experiment;
  htmlout << html_footer;
  htmlout.close();
}
