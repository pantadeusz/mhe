#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

using namespace std; ///< bad practice, but useful in examples for MHE

/**
 * graph representing problem
 * */
using my_graph_t = vector<vector<double>>;
/*
list of the cities
*/
using solution_t = vector<int>;

int f(int x) {return x*x;}

function<double(solution_t)> cost_function_factory(my_graph_t tsp_problem)
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

void print_mat(my_graph_t graph)
{
    for (auto row : graph) {
        for (auto v : row) {
            cout << v << " ";
        }
        cout << endl;
    }
}

int main(int argc, char** argv)
{
    //test_if_it_works();
    auto problem = load_problem("example1.txt");
    print_mat(problem);
    auto cost_func = cost_function_factory(problem);
    solution_t sol_candidate = {0, 2, 1, 3, 4};
    cout << "cost for: ";
    for (auto c : sol_candidate)
        cout << c << " ";
    cout << "is " << cost_func(sol_candidate) << endl;
    return 0;
}