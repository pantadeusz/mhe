#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "tsp_problem.hpp"

using namespace std; ///< bad practice, but useful in examples for MHE


/**
 * function that calculates accurate solution for TSP
 * */
//work_point_t brute_force_tsp(my_graph_t problem, std::function<double(work_point_t)> cost);

auto brute_force_tsp = [](my_graph_t problem, function<double(work_point_t)> cost) -> work_point_t
{
    work_point_t best_p = generate_first_tsp_point(problem);
    double best_goal_val = cost(best_p);
    auto p = best_p; // current work point
    const auto p0 = p;
    do {
        if (cost(p) < best_goal_val) {
            best_goal_val = cost(p);
            best_p = p;
            cout << "found better: " << best_goal_val << endl;
        }
        p = get_next_point(p);
    } while (!(p == p0));
    return best_p;
};


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
    vector<pair<double, double>> cities_coordinates;
    auto problem = generate_random_problem((argc < 2)?5:stoi(argv[1]), [&cities_coordinates](auto coords) {cities_coordinates = coords;});
    cout << problem << endl;
    auto cost_func = cost_function_factory(problem);
    work_point_t sol_candidate = generate_random_tsp_point(problem);
    cout << "cost for: " << make_pair(problem, sol_candidate) << "is " << cost_func(sol_candidate) << endl;
    
    
    work_point_t best_solution =  brute_force_tsp(problem, cost_func);

//    cout << "best cost for: " << make_pair(problem, best_solution) << "is " << cost_func(best_solution) << endl;
    cout << "best cost for:\n" << make_pair(cities_coordinates, best_solution) << "is " << cost_func(best_solution) << endl;
    
    return 0;
}