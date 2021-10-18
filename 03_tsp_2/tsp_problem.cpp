#include "tsp_problem.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

std::function<double(solution_t)> cost_function_factory(my_graph_t tsp_problem)
{
    return [=](solution_t list_of_cities) -> double {
        double sum = 0;
        for (int i = 0; i < list_of_cities.size(); i++) {
            int city1 = list_of_cities[i], city2 = list_of_cities[(i + 1) % list_of_cities.size()];
            double cost = tsp_problem.at(city1).at(city2);
            cout << city1 << "->" << city2 << ":" << cost << endl;
            if (cost == 0.0) throw std::string("no connection between cities");
            sum = sum + cost;
        }
        return sum;
    };
}


void test_if_it_works()
{
    my_graph_t cities = {
        // 0    1    2    3    4
        {0.0, 1.0, 2.0, 5.0, 8.0}, // 0
        {1.0, 0.0, 4.0, 3.0, 9.0}, // 1
        {2.0, 4.0, 0.0, 2.0, 6.0}, // 2
        {5.0, 3.0, 2.0, 0.0, 3.0}, // 3
        {8.0, 9.0, 6.0, 3.0, 0.0}  // 4
    };
    // 1 + 4 + 2 + 3 + 8 =  20

    auto cost_func = cost_function_factory(cities);
    solution_t sol_candidate = {0, 2, 1, 3, 4};
    cout << "cost for: ";
    for (auto c : sol_candidate)
        cout << c << " ";
    cout << "is " << cost_func(sol_candidate) << endl;
}

my_graph_t load_problem(string fname)
{
    ifstream inputfile(fname);
    my_graph_t result;
    std::string line;
    while (std::getline(inputfile, line)) {
        vector<double> row;
        std::istringstream iss(line);
        double value;
        while (!iss.eof()) {
            iss >> value;
            row.push_back(value);
        }
        result.push_back(row);
    }
    return result;
}

my_graph_t load_problem_from_coordinates(std::vector<std::pair<double, double>> cities_list)
{
    auto dist = [](std::pair<double, double> a, std::pair<double, double> b) {
        double r;
        r = (a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second);
        return std::sqrt(r);
    };

    my_graph_t ret(cities_list.size(), std::vector<double>(cities_list.size()));
    for (int row = 0; row < cities_list.size(); row++) {
        for (int col = 0; col < cities_list.size(); col++) {
            ret[row][col] = ret[col][row] = (row == col) ? 0.0 : dist(cities_list[row], cities_list[col]);
        }
    }
    return ret;
}


std::ostream& operator<<(std::ostream& o, const my_graph_t graph)
{
    for (auto row : graph) {
        o << "[ ";
        for (auto v : row) {
            o << v << " ";
        }
        o << "]" << std::endl;
    }
    return o;
}

std::ostream& operator<<(std::ostream& o, const std::pair<my_graph_t, solution_t> graph_w_solution)
{
    /*

graph G {
  a1 -- b3 [ label="2" ];
  a1 -- b2;
}
    */

    for (auto e : graph_w_solution.second) {
        std::cerr << e << " ";
    }
    std::cerr << std::endl;
    o << "graph G {" << endl;
    // not directed!!
    for (int row = 0; row < graph_w_solution.first.size(); row++) {
        for (int col = 0; col < row; col++) { // graph_w_solution.first.at(row).size(); col++) {
            if (graph_w_solution.first.at(row).at(col) > 0) {
                bool style = false;
                for (int i = 0; i < graph_w_solution.second.size(); i++) {
                    if (((graph_w_solution.second.at(i) == row) && (graph_w_solution.second.at((i + 1) % graph_w_solution.second.size()) == col)) || ((graph_w_solution.second.at(i) == col) && (graph_w_solution.second.at((i + 1) % graph_w_solution.second.size()) == row))) {
                        style = true;
                        break;
                    }
                }
                o << row << " -- " << col << " [ label=\"" << graph_w_solution.first.at(row).at(col) << (style ? "\",color=red" : "\"") << " ]" << endl;
            }
        }
    }

    o << "}" << endl;
    return o;
}
