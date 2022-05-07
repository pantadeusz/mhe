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


auto arg = [](int argc, char** argv, std::string name, auto default_value) -> decltype(default_value) {
    using namespace std;
    string paramname = "";
    any ret = default_value;
    for (auto argument: vector<string>(argv, argv + argc)) {
        if ((argument.size() > 0) && (argument[0] == '-')) {
            if (paramname != "") {
                if (name == argument.substr(1)) ret = true;
            }
            paramname = argument.substr(1);
        } else if (name == paramname) {
            if (std::is_same_v<decltype(default_value), int>) ret = stoi(argument);
            else if (std::is_same_v<decltype(default_value), double>) ret = stod( argument );
            else if (std::is_same_v<decltype(default_value), char>) ret = argument.at(0);
            else if (std::is_same_v<decltype(default_value), bool>) ret = (argument=="true") || (argument=="1") || (argument=="yes");
            else ret = argument;
            paramname = "";
        }
    }
    return std::any_cast<decltype(default_value)>(ret);
};


using coordinates_t = std::array<double, 2>;
using solution_t = std::vector<int>;
using problem_t = std::vector<coordinates_t>;
/**
 * @brief helper operator for printing the solution
 *
 * @param o stream
 * @param sol solution to print on stream
 * @return std::ostream& the same stream as o
 */
std::ostream& operator<<(std::ostream& o, solution_t sol)
{
    for (int i = 0; i < sol.size(); i++) {
        o << (i == 0 ? "" : ", ") << sol[i];
    }
    return o;
}

/**
 * @brief substracting two vectors
 *
 * @param a
 * @param b
 * @return coordinates_t
 */
coordinates_t operator-(coordinates_t a, coordinates_t b)
{
    return {a[0] - b[0], a[1] - b[1]};
}
/**
 * @brief vector length
 *
 * @param p
 * @return double
 */
double length(coordinates_t p)
{
    return std::sqrt(p[0] * p[0] + p[1] * p[1]);
}

/**
 * @brief the cost function for the Travelling Salesman Problem. It calculates the cost of following the selected order in the list of cities represented by the problem.
 *
 * @param problem list of the cities
 * @param route the visitation order of cities
 * @return double the cost of following the route and comming back to the first city
 */
double tsp_problem_cost_function(const problem_t problem, const solution_t route)
{
    double distance = 0;
    for (int i = 0; i < route.size(); i++) {
        auto p1 = problem.at(route[i]);
        auto p2 = problem.at(route[(i + 1) % route.size()]);
        distance += length(p2 - p1);
    }
    return distance;
};

/**
 * @brief load the TSP problem from file or standard input. Each line represents one city. The city is represented by its coordinates.
 *
 * @param fname input filename. If this is empty, then we load from standard input
 * @return problem_t loaded array representing cities coordinates.
 */
problem_t load_problem(std::string fname)
{
    using namespace std;
    problem_t problem;
    istream *fst = &cin;
    ifstream f;
    if (fname != "") {
        f = ifstream(fname);
        fst = &f;
    }
    string line;
    while (getline(*fst, line)) {
        stringstream sline(line);
        double x, y;
        sline >> x >> y;
        problem.push_back({x, y});
    }
    return problem;
}

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
T calculate_full_review(T start_solution, std::function<double(T)> cost, std::function<T(T)> next_solution, std::function<bool(T)> term_condition, bool progress=false ) {
    using namespace std;
    auto solution = start_solution;
    auto  best_solution = solution;
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

void show_help() {
#ifndef __OPTIMIZE__
#define OPTIMIZED "optimized "
#else
#define OPTIMIZED ""
#endif
    std::cout << "# " << OPTIMIZED << "binary with date: " << __TIME__ << " " << __DATE__ << std::endl;
    std::cout << "-fname filename" << std::endl;
    std::cout << "-show_progress true/false" << std::endl;

}

int main(int argc, char** argv)
{
    using namespace std;

/// get arguments
    auto fname = arg(argc, argv, "fname", std::string(""));
    auto show_progress = arg(argc, argv, "show_progress", false);
/// do we need to show some help?
    if (arg(argc, argv, "h", false)) {
        show_help();
        return 0;
    }
    cout << "# fname = " << fname << ";" << std::endl;

/// load the problem at hand
    auto problem = load_problem(fname);
    for (auto p : problem) {
        cout << "# P: " << p[0] << " " << p[1] << endl;
    }

/// generate initial solution
    solution_t solution;
    for (int i = 0; i < problem.size(); i++) {
        solution.push_back(i);
    }

/// prepare important functions:
/// goal function for solution - how good or bad it is. We minimize this function
    function<double(solution_t)> goal = [problem](auto s) {
        return tsp_problem_cost_function(problem, s);
    };
/// generate next solution for the method
    function<solution_t(solution_t)> next_solution =
    [](auto s) {
        next_permutation(s.begin(), s.end());
        return s;
    };
/// what is the termination condition
    function<bool(solution_t)> term_condition = [solution](auto s) {
        return !(s == solution);
    };
/// run the full review method
    auto calculation_start = chrono::steady_clock::now();
    solution_t best_solution = calculate_full_review<solution_t>(solution, goal,next_solution, term_condition, show_progress);
    auto calculation_end = chrono::steady_clock::now();
/// show the result
    chrono::duration<double> calculation_duration = calculation_end-calculation_start;
    cout << "# best_cost: " << tsp_problem_cost_function(problem, best_solution) << " calculation_time: " << calculation_duration.count() << endl;
    cout << "# solution: " << best_solution << endl;
    return 0;
}
