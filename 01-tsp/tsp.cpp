#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using coordinates_t = std::array<double, 2>;
using solution_t = std::vector<int>;
using problem_t = std::vector<coordinates_t>;

std::ostream& operator<<(std::ostream& o, solution_t sol)
{
    for (int i = 0; i < sol.size(); i++) {
        o << (i == 0 ? "" : ", ") << sol[i];
    }
    return o;
}

coordinates_t operator-(coordinates_t a, coordinates_t b)
{
    return {a[0] - b[0], a[1] - b[1]};
}
double length(coordinates_t p)
{
    return std::sqrt(p[0] * p[0] + p[1] * p[1]);
}

double goal_function(const problem_t problem, const solution_t candidate_solution)
{
    double distance = 0;
    for (int i = 0; i < candidate_solution.size(); i++) {
        auto p1 = problem.at(candidate_solution[i]);
        auto p2 = problem.at(candidate_solution[(i + 1) % candidate_solution.size()]);
        distance += length(p2 - p1);
    }
    return distance;
};

problem_t load_problem(std::string fname)
{
    using namespace std;
    problem_t problem;
    if (fname == "") {
        string line;
        while (getline(cin, line)) {
            stringstream sline(line);
            double x, y;
            sline >> x >> y;
            problem.push_back({x, y});
        }
    }
    return problem;
}

int main(int argc, char** argv)
{
    using namespace std;
    map<string, string> params;
    vector<string> arguments(argv, argv + argc);
    string paramname = "";
    for (auto argument : arguments) {
        if ((argument.size() > 0) && (argument[0] == '-'))
            paramname = argument.substr(1);
        else
            params[paramname] = argument;
    }

    auto problem = load_problem(params["fname"]);
    for (auto p : problem) {
        cout << "P: " << p[0] << " " << p[1] << endl;
    }

    solution_t solution;
    for (int i = 0; i < problem.size(); i++) {
        solution.push_back(i);
    }

    auto  best_solution = solution;
    do {
       // cout << "ocena " << solution << ": " << goal_function(problem, solution) << endl;
        if (goal_function(problem, solution) < goal_function(problem, best_solution)){
            cout << "lepsze " << solution << ": " << goal_function(problem, solution) << endl;
            best_solution = solution;
        }
    } while (std::next_permutation(solution.begin(), solution.end()));
    cout << "ocena " << best_solution << ": " << goal_function(problem, best_solution) << endl;
    
    return 0;
}