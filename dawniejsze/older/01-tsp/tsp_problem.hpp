#ifndef ___TSP_PROBLEM____
#define ___TSP_PROBLEM____
#include <vector>
#include <ostream>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>


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
inline std::ostream& operator<<(std::ostream& o, solution_t sol)
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
inline coordinates_t operator-(coordinates_t a, coordinates_t b)
{
    return {a[0] - b[0], a[1] - b[1]};
}
/**
 * @brief vector length
 *
 * @param p
 * @return double
 */
inline double length(coordinates_t p)
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
inline double tsp_problem_cost_function(const problem_t problem, const solution_t route)
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
inline problem_t load_problem(std::string fname)
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

inline std::string vis(solution_t sol, problem_t problem) {
    using namespace std;
    /*
    graph G {
    A [ pos = "0,0!"]
    B [ pos = "10,10!"]
    A -- B
    C -- A
    }
    */
    stringstream out;
    out << "graph G {" << endl;
    int num = 0;
    for (auto [x,y] : problem) {
        out << num << " [ pos = \"" << x << "," << y << "!\" ] " << endl;
        num ++;
    }
    for (int i = 0; i < sol.size(); i ++) {
        out << sol[i] << " -- " << sol[(i+1)%sol.size()] << endl;
    }

    out << "}" << endl;
    return out.str();
}

#endif

