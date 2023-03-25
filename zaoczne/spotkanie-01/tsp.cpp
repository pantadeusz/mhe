
#include <iostream>

#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <iomanip>

using vec2d = std::array<double, 2>;

vec2d operator-(vec2d a, vec2d b) { return {a[0] - b[0], a[1] - b[1]}; }

double len(vec2d a) { return std::sqrt(a[0] * a[0] + a[1] * a[1]); }

using problem_t = std::vector<vec2d>;

class solution_t : public std::vector<int>
{
public:
    std::shared_ptr<problem_t> problem;

    static solution_t for_problem(std::shared_ptr<problem_t> problem_)
    {
        solution_t sol;
        sol.resize(problem_->size());
        std::generate(sol.begin(), sol.end(), [n = 0]() mutable { return n++; });
        sol.problem = problem_;
        return sol;
    }

    double goal() const
    {
        double sum_distance = 0;
        for (int i = 0; i < size(); i++) {
            auto& p = *problem;
            auto& t = *this;
            sum_distance += len(p[t[i]] - p[t[(i + 1) % size()]]);
        }
        return sum_distance;
    }

    solution_t start_from_zero() const {
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
};


std::ostream& operator<<(std::ostream& o, const vec2d v)
{
    o << "[ ";
    for (auto e : v)
        o << e << " ";
    o << "]";
    return o;
}
std::ostream& operator<<(std::ostream& o, const problem_t v)
{
    o << "{ ";
    for (auto e : v)
        o << e << " ";
    o << "}";
    return o;
}
std::ostream& operator<<(std::ostream& o, const solution_t v)
{
    o << "{ ";
    for (auto e : v)
        o << e << " ";
    o << "}";
    return o;
}

std::random_device rd;
std::mt19937 rgen(rd());

solution_t random_modify(solution_t current_point)
{
    using namespace std;
    uniform_int_distribution<int> distr(0, current_point.size() - 1);
    int a = distr(rgen);
    swap(current_point[a], current_point[(a + 1) % current_point.size()]);
    return current_point;
}

solution_t best_neighbour(solution_t current_point)
{
    using namespace std;
    std::vector<solution_t> neighbours;
    for (int i = 0; i < current_point.size(); i++) {
    }
    return current_point;
}

solution_t brute_force(solution_t start_point)
{
    auto solution = start_point;
    for (int i = 0; i < solution.size(); i++) {
        solution[i] = i;
    }
    auto best_solution = solution;
    int i = 0;
    do {
        if (solution.goal() <= best_solution.goal()) {
            best_solution = solution;
            std::cout << (i++) << " " << solution << "  " << solution.goal() << " *** " << best_solution << "  " << best_solution.goal() << std::endl;
        }
    } while (std::next_permutation(solution.begin(), solution.end()));
    return best_solution;
}

solution_t random_hillclimb(solution_t solution)
{
    for (int i = 0; i < 5040; i++) {
        auto new_solution = random_modify(solution);
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            std::cout << i << " " << solution << "  " << solution.goal() << std::endl;
        }
    }
    return solution;
}

solution_t shortest_distance(solution_t solution)
{
    solution_t result = solution;
    auto& problem = *solution.problem;
    solution.erase(solution.begin());
    for (int i = 1; i < result.size(); i++) {
        auto found = std::min_element(solution.begin(), solution.end(), [&](auto a, auto b) {                    double dist_a = len(problem[result[i-1]] - problem[a]);                    double dist_b = len(problem[result[i-1]] - problem[b]);                    return dist_a < dist_b; });
        result[i] = *found;
        solution.erase(found);
    }
    return result;
}

std::ostream& print_solution_for_graphviz(std::ostream& o, const solution_t v)
{
    auto pow_modulo = [](unsigned int a, unsigned int b, unsigned int mod) {
        unsigned int result = 1;
        for (int i = 1; i < b; i++) result = (result * b) % mod;
        return result;
    };

    o << "digraph {" << std::endl;
    o << "graph [pad=\"0.212,0.055\" bgcolor=lightgray]" << std::endl;
    o << "node [style=filled]" << std::endl;

    for (auto e : v) {
        auto x = v.problem->at(e)[0];
        auto y = v.problem->at(e)[1];
        o << e << " [pos=\"" << x << "," << y << "!\" fillcolor=\"#" << 
        std::hex << std::setfill('0') << std::setw(6) 
        << (pow_modulo(7, e + 10, 0x0ffffff+2) % 0x0ffffff) << "\"]" << std::endl;
    }
    for (int i = 0; i < v.size(); i++)
        o << v[i] << " -> " << v[(i + 1) % v.size()] << std::endl;
    o << "}" << std::endl;
    return o;
}


problem_t random_problem(int n, double w, double h) {
    std::uniform_real_distribution<double> w_distr(0.0,w);
    std::uniform_real_distribution<double> h_distr(0.0,h);
    problem_t problem;
    for (int i = 0; i < n; i++) problem.push_back({w_distr(rgen), h_distr(rgen)});
    return problem;
}

int main()
{
    using namespace std;
    problem_t tsp_problem = {{1.3, 1}, {2.4, 1}, {1.5, 2}, {3.1, 1}, {3.2, 7}, {3.3, 9}, {1.4, 4}};
    auto solution = solution_t::for_problem(make_shared<problem_t>(tsp_problem));
    std::shuffle(solution.begin(), solution.end(), rgen);
    std::cout << tsp_problem << std::endl;
    std::cout << solution << "Start:  " << solution.goal() << std::endl;
    /*solution = random_hillclimb(solution); */
    solution = brute_force(solution);
    /* solution = shortest_distance(solution); */
    //*  { 6 0 3 5 2 1 4 }  30.9289 --  19.3343 // { 6 5 4 3 1 0 2 }  18.1745 -- bruteforce */
    std::cout << solution << "Result  " << solution.goal() << std::endl;
    print_solution_for_graphviz(std::cout, solution.start_from_zero());
    return 0;
}
