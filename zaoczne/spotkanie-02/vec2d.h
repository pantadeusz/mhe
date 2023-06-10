//
// Created by pantadeusz on 3/25/2023.
//

#ifndef MHE_VEC2D_H
#define MHE_VEC2D_H
#include <array>
#include <cmath>
#include <iostream>

namespace mhe {

    using vec2d = std::array<double, 2>;

    inline vec2d operator-(vec2d a, vec2d b) { return {a[0] - b[0], a[1] - b[1]}; }

    inline double len(vec2d a) { return std::sqrt(a[0] * a[0] + a[1] * a[1]); }

    inline std::ostream &operator<<(std::ostream &o, const vec2d v) {
        o << "[ ";
        for (auto e: v)
            o << e << " ";
        o << "]";
        return o;
    }

} // mhe

#endif //MHE_VEC2D_H
