#ifndef __SOLVER_BRUTE_FORCE_HPP___
#define __SOLVER_BRUTE_FORCE_HPP___

auto brute_force = [](
            auto generator,
            auto next_sol,
            auto goal
            ) {
  using namespace std;
  auto sol = generator();
  auto first_solution = sol;
  auto best_solution = sol;
  do {
    sol = next_sol(sol);
    if (goal(sol) < goal(best_solution)) {
      best_solution = sol;
    }
  } while (!(sol == first_solution)); // go to the first one
  return best_solution;
};

#endif
