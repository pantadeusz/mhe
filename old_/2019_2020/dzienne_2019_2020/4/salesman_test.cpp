#include "salesman.hpp"
#include <iostream>

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
