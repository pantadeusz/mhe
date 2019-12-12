#ifndef __SALESMAN_IMPL_SIMULATEDANNEALING_HPP___
#define __SALESMAN_IMPL_SIMULATEDANNEALING_HPP___
#include <functional>
#include <list>
#include <random>

/**
 * Simulated annealing algorithm to the traveling salesman problem.
 * */
template <class solution_t, class generator_t>
solution_t simulated_annealing(solution_t problem, generator_t generator,
                               const int iterations = 1000,
                               std::function<double(int)> T = [](int k) {
                                 return 4000000.0 / (double)k;
                               }) {
  using namespace std;
  list<solution_t> s;
  s.push_back(problem);
  // there must be more cities than 1
  for (int k = 1; k <= iterations; k++) {
    auto new_solution = s.back().generate_random_neighbour(1, generator);
    if (new_solution.goal() < s.back().goal()) {
      s.push_back(new_solution);
    } else {
      double u = uniform_real_distribution<double>(0.0, 1.0)(generator);
      auto f_t_k = new_solution.goal();
      auto f_s_k_1 = s.back().goal();
      if (u < exp(-abs(f_t_k - f_s_k_1) / T(k))) {
        s.push_back(new_solution);
      } else {
        s.push_back(s.back()); // for compatibility with pseudocode
      }
    }
  };

  // for (auto e: s) {
  //   // static int i = 0;
  // // i++;
  //   // cerr << i << " " << (e.get_solution().goal()/1000.0) << endl;
  // }
  return *min_element(s.begin(), s.end(),
                      [](auto a, auto b) { return (a.goal() < b.goal()); });
}

#endif
