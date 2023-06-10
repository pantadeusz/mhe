//
// Created by pantadeusz on 3/25/2023.
//

#include "problem_t.h"

#include "vec2d.h"

#include <vector>
#include <iostream>


namespace mhe {

    problem_t generate_problem(int size, double w, double h, std::mt19937 &rgen) {
        std::uniform_real_distribution<double> w_distr(0.0, w);
        std::uniform_real_distribution<double> h_distr(0.0, h);
        problem_t problem;
        for (int i = 0; i < size; i++) {
            problem.push_back({w_distr(rgen), h_distr(rgen)});
        }
        return problem;
    }

    std::ostream &operator<<(std::ostream &o, const problem_t v) {
        o << "{ ";
        for (auto e: v)
            o << e << " ";
        o << "}";
        return o;
    }
}

