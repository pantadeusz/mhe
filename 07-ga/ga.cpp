#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <memory>
#include <random>

std::random_device rd;
std::mt19937_64 rd_generator(rd);

struct city_t {
    std::vector<double> coordinates;
};

using problem_t = std::vector<city_t>;
class solution_t : public std::vector<int> {
    public:
        std::shared_ptr<problem_t> problem_p;
};


std::vector<double> operator*(const std::vector<double> a, const double b) {
    std::vector<double> result = a;
    for (auto &x:result) x = x * b;
    return result;
}
std::vector<double> operator+(const std::vector<double> a, const std::vector<double> b) {
    assert(a.size() == b.size());
    std::vector<double> result = a;
    for (int i = 0; i < b.size(); i++) result[i] += b.at(i);
    return result;
}
std::vector<double> operator-(const std::vector<double> a, const std::vector<double> b) {
    return a + (b*-1.0);
}

double count_cost(const problem_t &problem, const solution_t &solution) {
    double length = 0.0;
    for (int i = 0 ; i < solution.size(); i++) {
        auto v = problem[solution[i]].coordinates 
            - problem[solution[(i+1)%solution.size()]].coordinates;
        double l = 0;
        for (auto &x : v) l += x*x;
        length += std::sqrt(l);
    }
    return length;
}

double fitness(const solution_t solution) {
    return 1.0 / (1.0 + count_cost(*solution.problem_p, solution));
}

solution_t mutation(const solution_t &solution) {
    std::uniform_int_distribution<int> distr(0,solution.size()-1);
    int a(distr(rd_generator)), b(distr(rd_generator));
    auto ret = solution;
    std::swap(ret[a], ret[b]);
    return ret;
}


int main (int argc, char *argv) {

    return 0;
}