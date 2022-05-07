#ifndef _____tabu
#define _____tabu
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
T calculate_tabu(T start_solution,
                 std::function<double(T)> cost,
                 std::function<std::vector<T>(T)> get_neighbors,
                 std::function<bool(T)> term_condition,
                 int max_tabu_size = 100,
                 bool progress=false ) {
    using namespace std;
    auto solution = start_solution;
    auto best_solution = solution;
    std::list<T> tabu_list;
    std::set<T> tabu_set;
    int iteration_counter = 0;
    auto is_not_in_tabu = [&](T s) -> bool {
        return tabu_set.count(s) == 0;
    };

    do {
        auto neighbors = get_neighbors(solution);
        vector<T> good_neighbors;
        copy_if (neighbors.begin(), neighbors.end(), std::back_inserter(good_neighbors), is_not_in_tabu);
        if (good_neighbors.size() == 0) {
            cerr << "empty list..." << endl;
            break;
        }
        solution = *min_element(good_neighbors.begin(), good_neighbors.end(), [&](auto a, auto b) -> int {
            return (cost(a) < cost(b)) ;
        });
        tabu_list.push_back(solution);
        tabu_set.insert(solution);
        if (tabu_list.size() > max_tabu_size) {
            tabu_set.erase(tabu_list.front());
            tabu_list.pop_front();
        }
        if ((cost(solution) < cost(best_solution)) || (iteration_counter == 0)) {
            if (progress) cout << (iteration_counter-1) << " " << cost(best_solution) << " # " << solution << endl;
            best_solution = solution;
            if (progress) cout << iteration_counter << " " << cost(best_solution) << " # " << solution << endl;
        }
        iteration_counter++;
    } while (term_condition(solution));
    if (progress) cout << iteration_counter << " " << cost(best_solution) << " # " << solution << endl;
    return best_solution;
}




#endif
