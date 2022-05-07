#ifndef _____full_revision
#define _____full_revision

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


/**
 * @brief full review method that will use goal as a measure how bad is the solution (minimization).
 *
 * @tparam T the type of the solution
 * @param start_solution first solution to check
 * @param cost the goal function. Here it is cost function
 * @param next_solution method that will generate the next solution based on the current one
 * @param term_condition terminate iterating when this value is false
 * @param progress show progress on the console per iteration in the format suitable for gnuplot
 * @return T the best solution found so far
 */
template <typename T>
T calculate_full_review(T start_solution, std::function<double(T)> cost,
                        std::function<T(T)> next_solution, std::function<bool(T)> term_condition, bool progress=false ) {
    using namespace std;
    auto solution = start_solution;
    auto best_solution = solution;
    int iteration_counter = 0;
    do {
        if ((cost(solution) < cost(best_solution)) || (iteration_counter == 0)) {
            if (progress) cout << (iteration_counter-1) << " " << cost(best_solution) << " # " << solution << endl;
            best_solution = solution;
            if (progress) cout << iteration_counter << " " << cost(best_solution) << " # " << solution << endl;
        }
        iteration_counter++;
        solution = next_solution(solution);
    } while (term_condition(solution));
    if (progress) cout << iteration_counter << " " << cost(best_solution) << " # " << solution << endl;
    return best_solution;
}

#endif
