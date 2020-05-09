#ifndef __PROBLEM_TSP___HPP_
#define __PROBLEM_TSP___HPP_

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>

class city_t {
public:
  double x, y;
  std::string name;
  city_t operator-(const city_t c) { return {x - c.x, y - c.y, name}; };
  double len() { return std::sqrt(x * x + y * y); }
};

class tsp_solution_t {
public:
  std::shared_ptr<std::vector<city_t>>
      problem;                    // to jest zadanie do rozwiazania
  std::vector<int> cities_to_see; // to jest rozwiazanie
};

/**
 * Pozwala na wczytanie z standardowego wejścia definicję problemu
 * */
std::istream &operator>>(std::istream &os, tsp_solution_t &problem);
std::ostream &operator<<(std::ostream &os, const tsp_solution_t &solution);
bool operator==(const tsp_solution_t &a, const tsp_solution_t &b);

/**
 * Oblicza wartość funkcji celu - minimalizujemy ją.
 * */
double goal(tsp_solution_t sol);
/**
 * generuje następne poprawne rozwiazanie
 * */
tsp_solution_t next_solution(tsp_solution_t sol);
tsp_solution_t randomize_solution(tsp_solution_t initial_problem);

std::vector<tsp_solution_t> get_close_solutions(const tsp_solution_t sol0);
#endif