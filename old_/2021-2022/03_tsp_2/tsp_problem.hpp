#ifndef __TSP_PROBLEM_IMPLEMENTATION___
#define __TSP_PROBLEM_IMPLEMENTATION___

#include <functional>
#include <tuple>
#include <vector>

/**
 * graph representing problem
 * */
using my_graph_t = std::vector<std::vector<double>>;
/*
list of the cities
*/
using work_point_t = std::vector<int>;


std::function<double(work_point_t)> cost_function_factory(my_graph_t tsp_problem);

my_graph_t generate_random_problem(
    int n,
    std::function < void(std::vector<std::pair<double, double>>)> on_cities = [](auto cities_list) {});


my_graph_t load_problem(std::string fname);

my_graph_t load_problem_from_coordinates(std::vector<std::pair<double, double>> cities_list);

work_point_t generate_random_tsp_point(my_graph_t problem);

work_point_t generate_first_tsp_point(my_graph_t problem);
work_point_t get_next_point(work_point_t p);
bool operator==(work_point_t a, work_point_t b);


work_point_t brute_force_tsp(my_graph_t problem, std::function<double(work_point_t)> cost);



std::ostream& operator<<(std::ostream& o, const my_graph_t graph);

std::ostream& operator<<(std::ostream& o, const std::pair<my_graph_t, work_point_t> graph_w_solution);


#endif
