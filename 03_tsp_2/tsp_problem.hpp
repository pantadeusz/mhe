#ifndef __TSP_PROBLEM_IMPLEMENTATION___
#define __TSP_PROBLEM_IMPLEMENTATION___

#include <functional>
#include <vector>
#include <tuple>

/**
 * graph representing problem
 * */
using my_graph_t = std::vector<std::vector<double>>;
/*
list of the cities
*/
using solution_t = std::vector<int>;


std::function<double(solution_t)> cost_function_factory(my_graph_t tsp_problem);



my_graph_t load_problem(std::string fname);

my_graph_t load_problem_from_coordinates(std::vector<std::pair<double,double>> cities_list);


std::ostream &operator<<(std::ostream &o, const my_graph_t graph);

std::ostream &operator<<(std::ostream &o, const std::pair<my_graph_t,solution_t> graph_w_solution);


#endif
