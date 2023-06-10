//
// Created by pantadeusz on 3/25/2023.
//

#ifndef MHE_SOLUTION_T_H
#define MHE_SOLUTION_T_H

#include "problem_t.h"

#include <iostream>
#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <iomanip>
#include <list>
#include <set>


namespace mhe {

    class solution_t : public std::vector<int> {
    public:
        std::shared_ptr<problem_t> problem;

        static solution_t for_problem(std::shared_ptr<problem_t> problem_) ;
        double goal() const ;
        solution_t start_from_zero() const ;
        solution_t random_modify(std::mt19937 &rgen) const ;
        std::vector<solution_t> generate_neighbours() const ;
        solution_t best_neighbour() const ;

        static solution_t random_solution(problem_t tsp_problem, std::mt19937 &rgen) ;
    };



    std::ostream &operator<<(std::ostream &o, const solution_t v);


} // mhe

#endif //MHE_SOLUTION_T_H
