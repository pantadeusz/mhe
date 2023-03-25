#ifndef __SALESMAN_IMPL_HILLCLIMB_HPP___
#define __SALESMAN_IMPL_HILLCLIMB_HPP___

#include <memory>

/**
 * Hill climbing algorithm - randomized version
 * */
template <class solution_t, class generator_t>
solution_t hillclimb(solution_t solution, generator_t generator,
                     const int iterations = 1000) {
  using namespace std;
  // there must be more cities than 1
  for (int iteration = 0; iteration < iterations; iteration++) {
    auto new_solution = solution.generate_random_neighbour(1, generator);
    if (new_solution.goal() < solution.goal()) {
      solution = new_solution;
    }
  };

  return solution;
}

/**
 * Hill climbing algorithm - randomized version
 * */
template <class solution_t>
solution_t hillclimb_deteriministic(solution_t solution,
                                    const int iterations = 1000) {
  using namespace std;
  // there must be more cities than 1
  for (int iteration = 0; iteration < iterations; iteration++) {
    auto neighbours = solution.generate_neighbours();
    auto current_best = neighbours.back();
    for (auto &sol : neighbours) {
      if (sol.goal() < current_best.goal()) {
        current_best = sol;
      }
    }
    if (current_best.goal() < solution.goal()) {
      solution = current_best;
    } else {
      //       cerr << "found solution earlier " << iteration << " (before "
      //            << iterations << ")" << endl;
      return solution;
    }
  };

  return solution;
}

#endif
