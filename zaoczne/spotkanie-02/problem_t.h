//
// Created by pantadeusz on 3/25/2023.
//

#ifndef MHE_PROBLEM_T_H
#define MHE_PROBLEM_T_H

#include "vec2d.h"

#include <vector>
#include <iostream>
#include <random>
namespace mhe {
    using problem_t = std::vector<vec2d>;

    problem_t generate_problem(int size, double w, double h, std::mt19937 &rgen);

    std::ostream &operator<<(std::ostream &o, const problem_t v);
}

#endif //MHE_PROBLEM_T_H
