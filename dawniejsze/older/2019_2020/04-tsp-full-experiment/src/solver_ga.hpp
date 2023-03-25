#ifndef __SOLVER__GENETIC_ALGORITHM_HPP____
#define __SOLVER__GENETIC_ALGORITHM_HPP____
#include <iostream>

auto genetic_algorithm = [](auto init_population, auto selection,
                            auto crossover, //
                            auto mutation, auto fitness, auto term_condidtion) {
  using namespace std;
  std::vector<double> fitnesses;
  auto population = init_population();
  fitnesses.clear();
  fitnesses.reserve(population.size());
  for (auto &specimen: population) fitnesses.push_back(fitness(specimen)); 
  while (!term_condidtion(population, fitnesses)) {
    auto parents = selection(population, fitnesses);
    auto offspring = crossover(parents);
    offspring = mutation(parents);
    population = offspring;
    fitnesses.clear();
    fitnesses.reserve(population.size());
    for (auto &specimen: population) fitnesses.push_back(fitness(specimen)); 
  }
  return population;
};

#endif
