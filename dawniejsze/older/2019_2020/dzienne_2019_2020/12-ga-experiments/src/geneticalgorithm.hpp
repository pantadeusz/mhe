#ifndef __SALESMAN_IMPL_GENETIC_ALGORITHM_HPP___
#define __SALESMAN_IMPL_GENETIC_ALGORITHM_HPP___

#include <algorithm>
#include <memory>
#include <vector>

/**
 * This is the most generic genetic algoritm implementation.

 * @arg initial_population the collection with [] operator of specimens
 * @arg fitness_f the function that calculates the fitness. It takes the
 specimen as an argument
 * @arg selection_f the selection function that gets the population (vector of
 the specimens ) and returns the index in the population
 * @arg crossover_f function performing crossover. It takes 2 arguments (the two
 specimens) and returns the two crossed over specimens
 * @arg mutation_f the mutation function that takes one specimen and returns
 also one specimen
 * @arg term_condition_f function that takes the population and the iteration
 index, and returns the true (continue) or false (finish)
 *
 * @return the best specimen
*/
auto genetic_algorithm = [](auto initial_population, auto &fitness_f,
                            auto &selection_f, auto &crossover_f, auto &mutation_f,
                            auto &term_condition_f) {
  using namespace std;

  auto population = initial_population;
  int iteration = 0;
  vector<double> fit(initial_population.size()); ///< list of fitnesses
  #pragma omp parallel for
  for (unsigned i = 0; i < fit.size(); i++)
    fit[i] = fitness_f(population[i]); ///< calculate fitnesses

  while (term_condition_f(population, fit, iteration++)) {
    decltype(initial_population) parents(initial_population.size());  ///< parents selected
    decltype(initial_population) children(initial_population.size()); ///< offspring
    /// select speciments
    // (option) #pragma omp parallel for
    for (unsigned int i = 0; i < initial_population.size(); i++) {
      parents[i] = population[selection_f(fit, iteration)];
    }
    /// do the crossover
    for (int i = 0; i < (int)initial_population.size() - 1; i += 2) {
      auto [a, b] = crossover_f(parents[i], parents[i + 1]);
      children[i] = a;
      children[i+1] = b;
    }
    /// do the mutation
    for (int i = 0; i < (int)initial_population.size() - 1; i += 2) {
      children[i] = mutation_f(children[i]);
    }
    population = children;
    #pragma omp parallel for
    for (unsigned i = 0; i < fit.size(); i++)
      fit[i] = fitness_f(population[i]); ///< calculate fitnesses
  }
  /// return the best specimen of all
  return *std::max_element(
      population.begin(), population.end(),
      [&](auto &l, auto &r) { return fitness_f(l) < fitness_f(r); });
};

#endif
