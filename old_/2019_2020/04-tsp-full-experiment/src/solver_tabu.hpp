#ifndef __SOLVER__TABU_SEARCH_HPP____
#define __SOLVER__TABU_SEARCH_HPP____
#include <iostream>
#include <list>
#include <iostream>
#include <algorithm>

auto tabu_search = [](auto gen_random_sol, auto get_close_solutions, auto goal,
                      const int max_iterations = 1000, int tabu_size = 100) {
  using namespace std;
  auto sol = gen_random_sol();
  std::list<decltype(sol)> tabu;
  auto best_solution = sol;
  for (int i = 0; i < max_iterations; i++) {
    auto neighbours = get_close_solutions(sol);
    neighbours.erase(std::remove_if(neighbours.begin(), 
                              neighbours.end(),
                              [&tabu](auto x){return std::find(std::begin(tabu), std::end(tabu), x) != std::end(tabu);}),
               neighbours.end());
    //std::cout << "[T] acc_n " << neighbours.size() << " " << goal(sol) << " " << goal(best_solution) << std::endl;
    if (neighbours.size() == 0) {
      break; // nie mozemy nigdzie przejsc
    }
    auto best_neighbour = *max_element(
        std::begin(neighbours), std::end(neighbours),
        [&goal](auto a, auto b) { return goal(a) > goal(b); });
    sol = best_neighbour;
    tabu.push_back(sol);
    if (tabu.size() > tabu_size)
      tabu.pop_front();
    if (goal(sol) < goal(best_solution)) {
      best_solution = sol;
    }
  }
  return best_solution;
};

#endif
