#ifndef __SALESMAN_IMPL_TABUSEARCH_HPP___
#define __SALESMAN_IMPL_TABUSEARCH_HPP___

/**
 * tabu search
 */
template <class solution_t>
solution_t tabusearch(solution_t problem0, const int iterations = 1000,
                      const int max_tabu_size = 50) {
  using namespace std;
  list<solution_t> tabu_list; // tabu
  tabu_list.push_back(problem0);
  solution_t global_best = tabu_list.back();
  // repeat
  for (int iteration = 0; iteration < iterations; iteration++) {
    auto neighbours = tabu_list.back().generate_neighbours();
    // delete elements that are in tabu
    neighbours.erase(std::remove_if(neighbours.begin(), neighbours.end(),
                                    [&tabu_list](const auto &te) {
                                      for (auto &e : tabu_list)
                                        if (e == te)
                                          return true;
                                      return false;
                                    }),
                     neighbours.end());

    if (neighbours.size() <= 0) {
      // impossible to find next solution
      if (tabu_list.size() > 1)
        tabu_list.pop_front();
      else
        return global_best;
    } else {
      // find best neighbour
      auto current_best = neighbours.back();
      for (auto &sol : neighbours) {
        if (sol.goal() < current_best.goal()) {
          current_best = sol;
        }
      }
      // dodajemy lepszego
      tabu_list.push_back(current_best);
      if (current_best.goal() < global_best.goal()) {
        global_best = current_best;
      }

      // update global best
      if ((int)tabu_list.size() >= max_tabu_size)
        tabu_list.pop_front();
    }
  }
  return global_best;
}

#endif
