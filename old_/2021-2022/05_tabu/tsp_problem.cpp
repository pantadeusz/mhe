#include "tsp_problem.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>

using namespace std;

function<double(work_point_t)> cost_function_factory(my_graph_t tsp_problem)
{
    return [=](work_point_t list_of_cities) -> double {
        double sum = 0;
        for (int i = 0; i < list_of_cities.size(); i++) {
            int city1 = list_of_cities[i], city2 = list_of_cities[(i + 1) % list_of_cities.size()];
            double cost = tsp_problem.at(city1).at(city2);
            if (cost == 0.0) throw string("no connection between cities");
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
    work_point_t sol_candidate = {0, 2, 1, 3, 4};
    cout << "cost for: ";
    for (auto c : sol_candidate)
        cout << c << " ";
    cout << "is " << cost_func(sol_candidate) << endl;
}

my_graph_t load_problem(string fname)
{
    ifstream inputfile(fname);
    my_graph_t result;
    string line;
    while (getline(inputfile, line)) {
        vector<double> row;
        istringstream iss(line);
        double value;
        while (!iss.eof()) {
            iss >> value;
            row.push_back(value);
        }
        result.push_back(row);
    }
    return result;
}

my_graph_t load_problem_from_coordinates(vector<pair<double, double>> cities_list)
{
    auto dist = [](pair<double, double> a, pair<double, double> b) {
        double r;
        r = (a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second);
        return sqrt(r);
    };

    my_graph_t ret(cities_list.size(), vector<double>(cities_list.size()));
    for (int row = 0; row < cities_list.size(); row++) {
        for (int col = 0; col < cities_list.size(); col++) {
            ret[row][col] = ret[col][row] = (row == col) ? 0.0 : dist(cities_list[row], cities_list[col]);
        }
    }
    return ret;
}

random_device dandom_dev;
mt19937 rand_gen(dandom_dev());

my_graph_t generate_random_problem(int n, function<void(vector<pair<double, double>>)> on_cities)
{
    uniform_int_distribution<int> distr(0, n * 2);
    vector<pair<double, double>> cities_list;
    for (int i = 0; i < n; i++) {
        cities_list.push_back({(double)distr(rand_gen), (double)distr(rand_gen)});
    }
    on_cities(cities_list);
    return load_problem_from_coordinates(cities_list);
}


work_point_t generate_random_tsp_point(my_graph_t problem)
{
    vector<int> cities_to_take;
    work_point_t result;
    for (int i = 0; i < problem.size(); i++)
        cities_to_take.push_back(i);
    while (cities_to_take.size()) {
        uniform_int_distribution<int> distr(0, cities_to_take.size() - 1);
        int idx = distr(rand_gen);
        result.push_back(cities_to_take.at(idx));
        cities_to_take.erase(cities_to_take.begin() + idx);
    }
    return result;
}

std::vector<work_point_t> generate_tsp_point_neighbours(const work_point_t p)
{
    std::vector<work_point_t> result(p.size());
    for (int i = 0; i < p.size(); i++) {
        result[i] = p;
        auto &current_neighbour = result[i];
        swap(current_neighbour[i], current_neighbour[(i + 1) % p.size()]);
    }
    return result;
}


work_point_t generate_random_tsp_neighbour(work_point_t p)
{
    uniform_int_distribution<int> distr(0, p.size() - 2);
    int i = distr(rand_gen);
    std::swap(p[i], p[i + 1]);
    return p;
}


work_point_t generate_first_tsp_point(my_graph_t problem)
{
    work_point_t f;
    for (int i = 0; i < problem.size(); i++)
        f.push_back(i);
    return f;
}

work_point_t get_next_point(work_point_t p)
{
    next_permutation(p.begin(), p.end());
    return p;
}

bool operator==(work_point_t a, work_point_t b)
{
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) {
        if (a.at(i) != b.at(i)) return false;
    }
    return true;
}


ostream& operator<<(ostream& o, const my_graph_t graph)
{
    for (auto row : graph) {
        o << "[ ";
        for (auto v : row) {
            o << v << " ";
        }
        o << "]" << endl;
    }
    return o;
}

ostream& operator<<(ostream& o, const pair<my_graph_t, work_point_t> graph_w_solution)
{
    /*

graph G {
  a1 -- b3 [ label="2" ];
  a1 -- b2;
}
    */

    //    for (auto e : graph_w_solution.second) {
    //        cerr << e << " ";
    //    }
    //    cerr << endl;
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


std::ostream& operator<<(std::ostream& o, const std::pair<vector<pair<double, double>>, work_point_t> graph_w_solution)
{
    auto [coordinates, order] = graph_w_solution;
    for (int i = 0; i < order.size(); i++) {
        o << coordinates.at(order[i]).first << " " << coordinates.at(order[i]).second << " " << i << std::endl;
    }
    return o;
}
