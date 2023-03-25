#ifndef _____genetic_algorithm
#define _____genetic_algorithm

#include "global_random.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <any>
#include <chrono>
#include <list>
#include <set>



int selection_tournament(std::vector<double> fitnesses) {
    std::uniform_int_distribution<int> distr(0,fitnesses.size()-1);
    int idx1 = distr(random_generator);
    int idx2 = distr(random_generator);
    if (fitnesses[idx1] > fitnesses[idx2]) return idx1;
    return idx2;
}

template <typename T>
T calculate_genetic_algorithm(std::vector<T> initial_population,
                              std::function<double(T)> fitness,
                              std::function<int(std::vector<double>)> selection,
                              std::function<std::pair <T,T> (T,T)> crossover,
                              std::function         <   T    (T)   >        mutation,
                              std::function<bool(std::vector<double>, std::vector<T>)> term_condition,
                              bool progress=false ) {
    using namespace std;
    auto population = initial_population;
    auto calculate_fitness = [&](auto pop) {
        std::vector<double>  ret;
        for (auto e : pop) ret.push_back(fitness(e));
        return ret;
    };
    auto population_fit = calculate_fitness(population);
    
    while (term_condition(population_fit,population)) {
        vector<T> parents;
        vector<T> offspring;
        
        for (int i = 0; i < population.size(); i++) {
            parents.push_back(population[selection(population_fit)]);
        }
        for (int i = 0; i < parents.size(); i+= 2) {
            auto [a,b] = crossover(parents[i], parents[i+1]);
            offspring.push_back(a); offspring.push_back(b);
        }
        for (int i = 0; i < parents.size(); i++) {
            offspring[i] = mutation(offspring[i]);
        }
        
        population = offspring;
        population_fit = calculate_fitness(population);
    } ;
    //if (progress) cout << iteration_counter << " " << cost(best_solution) << " # " << solution << endl;
    return population[
        max_element(population_fit.begin(), population_fit.end())
        -population_fit.begin()];
}


#endif
