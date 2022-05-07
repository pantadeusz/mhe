#ifndef _____simulated_annealing
#define _____simulated_annealing

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


template <typename T>
T calculate_simulated_annealing(T start_solution,
                                std::function<double(T)> cost,
                                std::function<T(T)> next_solution,
                                std::function<double(int)> temperature,
                                std::function<bool(T)> term_condition,
                                bool progress=false ) {
    using namespace std;
    auto solution = start_solution;
    auto best_solution = solution;
    int iteration_counter = 1;
    do {
        auto neighbor = next_solution(solution);
        if (cost(neighbor) < cost(solution)) {
            solution = neighbor;
        } else {
            uniform_real_distribution<double> u(0,1);
            if (u(random_generator) < exp(-abs(cost(solution)-cost(neighbor))/temperature(iteration_counter))) {
                solution = neighbor;
            }
        }
        if (cost(solution) < cost(best_solution)) best_solution = solution;
        iteration_counter++;
        if (progress) cout << iteration_counter << " " << cost(solution) <<  " " << cost(best_solution) << " # SOL: " << solution << endl;

    } while (term_condition(solution));
    if (progress) cout << iteration_counter << " " << cost(best_solution) << " # " << solution << endl;
    return best_solution;
}


#endif
