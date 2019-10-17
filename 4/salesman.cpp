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
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <list>

#include "json.hpp"

#include "salesman_html_skel.cpp"

/**
 * @brief class representing single city on the map
 */
class city_t {
public:
  std::string name;
  double longitude;
  double latitude;

  /**
   * @brief calculate distance between two cities
   *
   * @param c2 second city to calculate distance to
   * @return double distance on the planet Earth
   */
  double distance(city_t &c2) {
    using namespace std;
    auto &[name2, lon2, lat2] = c2;
    auto R = 6371e3; // metres
    auto fi1 = latitude * M_PI / 180.0;
    auto fi2 = lat2 * M_PI / 180.0;
    auto deltafi = (lat2 - latitude) * M_PI / 180.0;
    auto deltalambda = (lon2 - longitude) * M_PI / 180.0;
    auto a = sin(deltafi / 2) * sin(deltafi / 2) +
             cos(fi1) * cos(fi2) * sin(deltalambda / 2) * sin(deltalambda / 2);
    auto c = 2 * atan2(sqrt(a), sqrt(1 - a));
    auto d = R * c;
    return d;
  }
};

/**
 * @brief the problem definition
 *
 * The cities we would like to see.
 */
class problem_t {
public:
  /**
   * @brief list of the cities to visit.
   */
  std::vector<city_t> cities;
};

/**
 * @brief the solution type
 *
 */
class solution_t {
public:
  /**
   * @brief smart pointer that have reference to the problem
   */
  std::shared_ptr<problem_t> problem;

  /**
   * @brief cities indices.
   * The order in wich we would like to see every city
   */
  std::vector<int> cities_to_see;

  /**
   * @brief Construct a new solution_t object
   *
   * This constructor will also generate new problem as a shared pointer so you
   * don't have to init the problem earlier.
   *
   * @param input_cities the cities list.
   */
  solution_t(const std::vector<city_t> input_cities)
      : problem(std::make_shared<problem_t>(problem_t{input_cities})),
        cities_to_see(input_cities.size()) {
    for (int i = 0; i < cities_to_see.size(); i++)
      cities_to_see[i] = i;
  };

  /**
   * @brief Construct a new empty solution_t object
   *
   * @param input_cities the cities list.
   */
  solution_t()
      : problem(std::make_shared<problem_t>(problem_t{std::vector<city_t>()})),
        cities_to_see(0){};
  /**
   * @brief Construct a new solution t object
   * It takes shared pointer to the problem definition. This way we don't have
   * to copy the problem, only smartpointer to it.
   * @param problem_ the problem smartpointer
   */
  solution_t(std::shared_ptr<problem_t> problem_)
      : problem(problem_), cities_to_see(problem_->cities.size()) {
    for (int i = 0; i < cities_to_see.size(); i++)
      cities_to_see[i] = i;
  };

  /**
   * @brief goal function value.
   *
   * It calculates the summary distance of the path following the solution
   * cities order. The traveller must come back to home.
   *
   * @return double lenght of the path.
   */
  double goal() const {
    double sum = 0.0;
    auto prev_city =
        cities_to_see.back(); // we must come back, so include the last city
    for (auto city : cities_to_see) { // now walk between cities
      sum += problem->cities[city].distance(problem->cities[prev_city]);
      prev_city = city;
    }
    return sum;
  };

  /**
   * @brief generates next possible solution
   *
   * @return problem_t the new solution to the problem
   */
  solution_t next_solution() const {
    auto p = *this;
    std::next_permutation(p.cities_to_see.begin() + 1, p.cities_to_see.end());
    return p;
  };

  /**
   * @brief This operator allows for checking if the solutions are the same and
   * the problem is the same
   *
   * @param other
   * @return true
   * @return false
   */
  bool operator==(const solution_t &other) const {
    if (this->problem !=
        other.problem) // is the problem the same (pointers the same)
      return false;
    if (other.cities_to_see.size() !=
        cities_to_see.size()) // do we have same number of cities
      throw std::invalid_argument(
          "cities count must be the same - problem in solution! check "
          "initializers and data consistency");
    for (int i = 0; i < cities_to_see.size(); i++) { // check the cities order
      if (other.cities_to_see[i] != cities_to_see[i])
        return false;
    }
    return true;
  }
};

/**
 * @brief prints solution to output stream
 *
 * overrides the << operator, so you can write ```cout << solution```
 */
std::ostream &operator<<(std::ostream &s, const solution_t &sol) {
  using namespace nlohmann;
  json j;
  j["cities"] = json::array();
  for (auto city : sol.cities_to_see) {
    j["cities"].push_back({sol.problem->cities[city].name,
                           sol.problem->cities[city].longitude,
                           sol.problem->cities[city].latitude});
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
std::istream &operator>>(std::istream &s, solution_t &sol) {
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

solution_t brute_force_find_solution(solution_t problem) {
  using namespace std;
  solution_t best = problem;
  solution_t problem0 = problem;
  do {
    if (best.goal() > problem.goal()) {
      best = problem;
    }
    problem = problem.next_solution();
  } while (!(problem == problem0));

  return best;
}

//// Reprezentacja porzadkowa

class alternative_solution_t {
public:
  std::vector<int> solution;
  std::shared_ptr<problem_t> problem;

  solution_t get_solution() {
    using namespace std;
    solution_t ret(problem);
    vector<int> indexes(solution.size());
    for (int i = 0; i < solution.size(); i++)
      indexes[i] = i;
    for (int i = 0; i < solution.size(); i++) {
      ret.cities_to_see[i] = indexes[solution[i]];
      indexes.erase(indexes.begin() + solution[i]); // remove i-th element
    }
    return ret;
  }
  void set_solution(solution_t s) {}
};

std::random_device seedsource;
std::default_random_engine generator(seedsource());

solution_t hillclimb(std::shared_ptr<problem_t> problem) {
  using namespace std;
  alternative_solution_t solution;
  solution.solution.resize(problem->cities.size());
  for (int i = 0; i < solution.solution.size(); i++) {
    std::uniform_int_distribution<int> distribution(
        0, solution.solution.size() - 1 - i);
    solution.solution[i] = distribution(generator);
  }
  solution.problem = problem;

  // there must be more cities than 1
  for (int iteration = 0; iteration < 100000; iteration++) {
    std::uniform_int_distribution<int> distribution(
        0, solution.solution.size() - 2); // modyfikujemy
    std::uniform_int_distribution<int> dist01(0, 1);
    int idx = distribution(generator);
    alternative_solution_t solution_candidate = solution;
    solution_candidate.solution[idx] = (solution_candidate.solution[idx] 
        + (2*dist01(generator)-1) 
        + solution.solution.size()-idx) % 
          (solution.solution.size()-idx);

    if (solution_candidate.get_solution().goal() < solution.get_solution().goal()) {
      solution = solution_candidate;
      cout << solution.get_solution().goal()/1000.0 << endl; 
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

    if (solution_candidate.get_solution().goal() < solution.get_solution().goal()) {
      solution = solution_candidate;
      cout << solution.get_solution().goal()/1000.0 << endl; 
    }
  };

  return solution.get_solution();
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

//  experiment = brute_force_find_solution(experiment);
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
