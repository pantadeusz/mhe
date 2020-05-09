#include "problem_tsp.hpp"

#include "json.hpp"
// dot -Kfdp -n -Tpng example.dot  > example.png
using nlohmann::json;

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

bool operator==(const tsp_solution_t &a, const tsp_solution_t &b) {
  if (a.problem != b.problem)
    return false;
  if (a.cities_to_see.size() != b.cities_to_see.size())
    return false;
  for (unsigned i = 0; i < a.cities_to_see.size(); i++) {
    if (a.cities_to_see[i] != b.cities_to_see[i])
      return false;
  }
  return true;
}

std::istream &operator>>(std::istream &os, tsp_solution_t &problem) {
  json data;
  int i = 0;
  os >> data;
  problem.cities_to_see.clear();
  problem.problem = std::make_shared<std::vector<city_t>>();
  for (auto e : data.at("problem")) {
    problem.problem->push_back(
        {x : e.at("x"), y : e.at("y"), name : e.at("name")});
    problem.cities_to_see.push_back(i++);
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const tsp_solution_t &solution) {
  for (auto e : solution.cities_to_see) {
    os << solution.problem->at(e).x << " " << solution.problem->at(e).y
       << solution.problem->at(e).name << std::endl;
  }
  return os;
}

/**
 * Oblicza wartość funkcji celu - minimalizujemy ją.
 * */
double goal(tsp_solution_t sol) {
  double distance = 0.0;
  auto p = sol.problem->at(sol.cities_to_see.at(0));
  for (int i = 1; i <= sol.problem->size(); i++) {
    distance +=
        (p - sol.problem->at(sol.cities_to_see[i % sol.problem->size()])).len();
    p = sol.problem->at(sol.cities_to_see[i % sol.problem->size()]);
  }
  return distance;
}

/**
 * generuje następne poprawne rozwiazanie
 * */
tsp_solution_t next_solution(tsp_solution_t sol) {
  std::next_permutation(sol.cities_to_see.begin(), sol.cities_to_see.end());
  return sol;
};

tsp_solution_t randomize_solution(tsp_solution_t initial_problem) {
  using namespace std;
  static random_device rd;
  static mt19937 gen(rd());
  // wygenerujmy sekwencję od 0 do liczba_miast-1
  vector<int> cities(initial_problem.problem->size());
  std::iota (std::begin(cities), std::end(cities), 0);
  // miasta do odwiedzenia - czyścimy
  initial_problem.cities_to_see.clear();
  // generujemy losową kolejność
  std::random_shuffle ( cities.begin(), cities.end(), [](int i){return gen()%i;});
  initial_problem.cities_to_see = cities;
  return initial_problem;
};

std::vector<tsp_solution_t> get_close_solutions(const tsp_solution_t sol0) {
  auto sol = sol0;
  std::vector<tsp_solution_t> ret;
  for (unsigned i = 0; i < sol.cities_to_see.size(); i++) {
    ret.push_back(sol);
  }

  // zamieniamy miejscami 2 ze sobą
  for (int i = 0; i < (int)sol.cities_to_see.size(); i++) {
    std::swap(ret.at(i).cities_to_see.at(i),
              ret.at(i).cities_to_see.at((i + 1) % sol.cities_to_see.size()));
  }
  return ret;
}
