//
// Created by pantadeusz on 3/25/2023.
//

#include "solution_t.h"

namespace mhe {

    solution_t solution_t::for_problem(std::shared_ptr<problem_t> problem_) {
        solution_t sol;
        sol.resize(problem_->size());
        std::generate(sol.begin(), sol.end(), [n = 0]() mutable { return n++; });
        sol.problem = problem_;
        return sol;
    }

    solution_t solution_t::random_solution(problem_t tsp_problem, std::mt19937 &rgen) {

        auto solution = solution_t::for_problem(make_shared<problem_t>(tsp_problem));
        std::shuffle(solution.begin(), solution.end(), rgen);
        return solution;
    }

    double solution_t::goal() const {
        double sum_distance = 0;
        for (int i = 0; i < size(); i++) {
            auto &p = *problem;
            auto &t = *this;
            sum_distance += len(p[t[i]] - p[t[(i + 1) % size()]]);
        }
        return sum_distance;
    }

    solution_t solution_t::start_from_zero() const {
        solution_t ret = *this;
        for (int i = 1; i < size(); i++) {
            if (at(i) == 0) {
                for (int j = 0; j < size(); j++) {
                    ret[j] = at((i + j) % size());
                }
                break;
            }
        }
        return ret;
    }

    solution_t solution_t::random_modify(std::mt19937 &rgen) const {
        using namespace std;
        solution_t current_point = *this;
        uniform_int_distribution<int> distr(0, size() - 1);
        int a = distr(rgen);
        std::swap(current_point[a], current_point[(a + 1) % current_point.size()]);
        return current_point;
    }

    std::vector<solution_t> solution_t::generate_neighbours() const {
        auto current_point = *this;
        std::vector<solution_t> result;
        for (int i = 0; i < current_point.size(); i++) {
            solution_t neighbour = current_point;
            std::swap(neighbour[i], neighbour[(i + 1) % current_point.size()]);
            result.push_back(neighbour);
        }
        return result;
    }


    solution_t solution_t::best_neighbour() const {
        auto current_point = *this;
        using namespace std;
        std::vector<solution_t> neighbours = current_point.generate_neighbours();
        return *std::max_element(neighbours.begin(), neighbours.end(), [](auto l, auto r) {
            return l.goal() > r.goal();
        });
    }


    std::ostream &operator<<(std::ostream &o, const solution_t v) {
        o << "{ ";
        for (auto e: v)
            o << e << " ";
        o << "}";
        return o;
    }


} // mhe