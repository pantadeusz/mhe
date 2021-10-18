#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "tsp_problem.hpp"

using namespace std; ///< bad practice, but useful in examples for MHE


int main(int argc, char** argv)
{
    //test_if_it_works();
//    auto problem = load_problem("example1.txt");
    auto problem = load_problem_from_coordinates({
        {0,1},
        {1,1},
        {1,2},
        {2,1},
        {1,0},
    });
    cout << problem << endl;
    auto cost_func = cost_function_factory(problem);
    solution_t sol_candidate = {0, 2, 1, 3, 4};
    cout << "cost for: ";
    for (auto c : sol_candidate)
        cout << c << " ";
    cout << "is " << cost_func(sol_candidate) << endl;
    cout << make_pair(problem, sol_candidate) << endl;
    return 0;
}