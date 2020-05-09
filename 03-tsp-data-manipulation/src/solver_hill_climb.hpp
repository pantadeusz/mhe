#ifndef __SOLVER__HILL_CLIMB_HPP____
#define __SOLVER__HILL_CLIMB_HPP____
#include <iostream>

auto hill_climbing = [](auto gen_random_sol, auto get_close_solutions,
                        auto goal, const int max_iterations = 10000) {
  using namespace std;
  int i = 0;
  auto sol = gen_random_sol();
  auto first_solution = sol;
  do {
    auto neighbours = get_close_solutions(sol);
    auto best_neighbour = *max_element(
        std::begin(neighbours), std::end(neighbours),
        [&goal](auto a, auto b) { return goal(a) > goal(b); });
    if (goal(sol) > goal(best_neighbour)) {
        //std::cout << "[HC] " << goal(sol) << " > " << goal(best_neighbour) << std::endl;
      sol = best_neighbour;
    } else { /// warunek zakonczenia
        //std::cout << "[HCRET] " << goal(sol) << " > " << goal(best_neighbour) << std::endl;
      return sol;
    }
  } while (i < max_iterations);
  return sol;
};

#endif
