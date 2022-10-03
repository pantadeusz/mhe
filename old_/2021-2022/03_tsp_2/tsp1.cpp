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
    //auto problem = load_problem_from_coordinates({
    //    {0,1},
    //    {1,1},
    //    {1,2},
    //    {2,1},
    //    {1,0},
    //});
    auto problem = generate_random_problem((argc < 2)?3:stoi(argv[1]));
    cout << problem << endl;
    auto cost_func = cost_function_factory(problem);
    work_point_t sol_candidate = generate_random_tsp_point(problem);
    cout << "cost for: " << make_pair(problem, sol_candidate) << "is " << cost_func(sol_candidate) << endl;
    
    
    work_point_t best_solution =  brute_force_tsp(problem, cost_func);

    cout << "best cost for: " << make_pair(problem, best_solution) << "is " << cost_func(best_solution) << endl;
    
    return 0;
}