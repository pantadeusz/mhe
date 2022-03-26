#ifndef __M_GA_____HPP___
#define __M_GA_____HPP___


#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <set>
#include <future>
//#include <openmp>


std::random_device rd;
std::mt19937 randgen(rd());


auto genetic_algorithm = [](
                             auto fitness,
                             auto population,
                             auto term_condition, // int iterations,
                             auto select,
                             auto crossover,
                             double p_crossover,
                             auto mutation,
                             double p_mutation,
                             std::function<void(int c, double dt)> on_statistics = [](int c, double dt) {},
                             std::function<void(int i, double current_goal_val, double goal_val)> on_iteration = [](int i, double current_goal_val, double goal_val) {}) {
    using namespace std;
    auto start = chrono::steady_clock::now();
    int iteration = 0;

    vector<double> population_fit(population.size());
    std::transform(population.begin(), population.end(), population_fit.begin(), fitness);
    std::cout << "GA!!\n"
              << std::endl;
    while (term_condition(population, population_fit, iteration++)) {
        decltype(population) new_pop(population.size());
        auto iterations_ = population.size();
        #pragma omp parallel for
        for (int c = 0; c < iterations_; c += 2) {
            uniform_real_distribution<double> should_mutate;
            uniform_real_distribution<double> should_crossover;
            auto parent1 = population.at(select(population_fit));
            auto parent2 = population.at(select(population_fit));
            auto [child1, child2] = (should_crossover(randgen) < p_crossover) ? crossover(parent1, parent2) : make_pair(parent1, parent2);
            if (should_mutate(randgen) < p_mutation) {
                child1 = mutation(child1);
            }
            if (should_mutate(randgen) < p_mutation) {
                child2 = mutation(child2);
            }
            new_pop[c + 0] = child1;
            new_pop[c + 1] = child2;
        }
        population = new_pop;

        auto task1f = [&,iterations_]() {
            for (int i = 0; i < iterations_ / 2; i++)
                population_fit[i] = fitness(population[i]);
            return 0;
        };

        auto task2f = [&,iterations_]() {
            for (int i = iterations_ / 2; i < iterations_; i++)
                population_fit[i] = fitness(population[i]);
            return 0;
        };
        //auto task1 = std::async(std::launch::async, task1f);
        //auto task2 = std::async(std::launch::async, task2f);
        //task1.get();
        //task2.get();
        std::transform(population.begin(), population.end(), population_fit.begin(), fitness);

        auto best_specimen_it = max_element(population_fit.begin(), population_fit.end(), [&](auto a, auto b) {
            return a < b;
        });
        on_iteration(iteration, *best_specimen_it, *best_specimen_it);
    }


    auto finish = chrono::steady_clock::now();
    chrono::duration<double> duration = finish - start;
    on_statistics(iteration, duration.count());
    return population;
};

#endif
