#ifndef __SALESMAN_IMPL_BRUTE_HPP___
#define __SALESMAN_IMPL_BRUTE_HPP___

#include <memory>

template <class solution_t>
solution_t brute_force_find_solution(solution_t problem0) {
  using namespace std;
  solution_t current = problem0;  // current solution
  solution_t best = problem0;     // best solution so far

  do {
    current = current.next_solution();
    if (best.goal() > current.goal())
      best = current;
  } while (!(current == problem0));

  return best;
}


#endif
