#ifndef __SALESMAN_IMPL_HILLCLIMB_HPP___
#define __SALESMAN_IMPL_HILLCLIMB_HPP___

#include <memory>

/**
 * Hill climbing algorithm - randomized version
 * */
template <class solution_t, class generator_t>
solution_t hillclimb(solution_t solution,
                     generator_t generator,
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


#endif

