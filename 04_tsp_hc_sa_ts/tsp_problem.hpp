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
list of the cities - indexes
*/
using work_point_t = std::vector<int>;

/**
 * generates cost function for TSP
 * */
std::function<double(work_point_t)> cost_function_factory(my_graph_t tsp_problem);

/**
 * this generates random problem (set of cities) for TSP
 * 
 * it will provide information about the cities coordinates via ```on_cities``` callback
 * */
my_graph_t generate_random_problem(
    int n,
    std::function < void(std::vector<std::pair<double, double>>)> on_cities = [](auto cities_list) {});

/**
 * thid function loads the graph from file
 * */
my_graph_t load_problem(std::string fname);

/**
 * this function loads list of cities and then generates graph
 * */
my_graph_t load_problem_from_coordinates(std::vector<std::pair<double, double>> cities_list);
/**
 * this function generates random path in graph.
 * */
work_point_t generate_random_tsp_point(my_graph_t problem);

/**
 * it will generate first solutino for TSP problem - it is the list of cities to see [0, 1, 2, ... ,n-1]
 * */ 
work_point_t generate_first_tsp_point(my_graph_t problem);

/**
 * this function will give the next point for TSP
 * */
work_point_t get_next_point(work_point_t p);

/**
 * this function allows to compare two solutions for TSP
 * */
bool operator==(work_point_t a, work_point_t b);


/**
 * output operator that allows to writing problem data to stream
 * */
std::ostream& operator<<(std::ostream& o, const my_graph_t graph);

/**
 * this will output the solution to the stream.
 * */
std::ostream& operator<<(std::ostream& o, const std::pair<my_graph_t, work_point_t> graph_w_solution);

//vector<pair<double, double>>
std::ostream& operator<<(std::ostream& o, const std::pair<std::vector<std::pair<double, double>>, work_point_t> graph_w_solution);

#endif
