#ifndef __SOLVER_HILL_CLIMB_ND__HPP___
#define __SOLVER_HILL_CLIMB_ND__HPP___

#include <random>
#include <iostream>

auto hill_climbing_nd = [](auto generate_random_solution,
auto get_close_solutions,
auto goal,
const int iterations = 100
    ) {
  using namespace std;
    using namespace std;
    static random_device rd;
    static mt19937 gen(rd());

  auto sol = generate_random_solution();
  
  auto first_solution = sol;
  for (int i = 0 ; i< iterations; i++ ){
    auto neighbours = get_close_solutions(sol);
    // losujemy jedno z dozwolonych rozwiązań
    uniform_int_distribution<> dist(0, neighbours.size() - 1);
    auto best_neighbour = neighbours.at(dist(gen)); 
    if (goal(sol) > goal(best_neighbour)) {
        sol = best_neighbour;
//        std::cout << "[HCND] " << i << " " << goal(sol) << endl;
    }
  };
  return sol;
};

#endif
