#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <vector>

#include "tp_args.hpp"

std::random_device rd;
std::mt19937_64 rd_generator(rd());

struct city_t {
    std::string name;
    // {latitude, longitude}
    std::array<double, 2> coordinates;
};

using problem_t = std::vector<city_t>;

class solution_t : public std::vector<int>
{
public:
    std::shared_ptr<problem_t> problem_p;
};


std::string utc(const std::time_t time)
{
    const auto l = std::string("yyyy-mm-ddThh:mm:ssZ").size();
    char timeString[l];
    std::strftime(timeString, l, "%FT%TZ", std::gmtime(&time));
    return timeString;
}

std::ostream& operator<<(std::ostream& o, const solution_t& solution)
{
    std::time_t time = std::time({});
    o << R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<gpx xmlns="http://www.topografix.com/GPX/1/1" xmlns:gpxx="http://www.garmin.com/xmlschemas/GpxExtensions/v3" xmlns:gpxtpx="http://www.garmin.com/xmlschemas/TrackPointExtension/v1" creator="Oregon 400t" version="1.1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd">
  <metadata>
    <link href="http://www.garmin.com">
      <text>Garmin International</text>
    </link>
    <time>)"
      << utc(time) << R"(</time>
  </metadata>
  <trk>
    <name>TSP Track</name>
    <trkseg>)";
    if (solution.size() > 0) {
        auto sol_element = solution.back();
        o << "<trkpt lat=\"" << solution.problem_p->at(sol_element).coordinates[0] << "\" lon=\"" << solution.problem_p->at(sol_element).coordinates[1] << "\">\n";
        o << "<ele>200</ele>\n";
        o << "<time>" << utc(time) << "</time>\n";
        o << "</trkpt>\n";
    }
    for (auto sol_element : solution) {
        o << "<trkpt lat=\"" << solution.problem_p->at(sol_element).coordinates[0] << "\" lon=\"" << solution.problem_p->at(sol_element).coordinates[1] << "\">\n";
        o << "<ele>200</ele>\n";
        o << "<time>" << utc(time) << "</time>\n";
        o << "</trkpt>\n";
    }
    o << R"(</trkseg>
  </trk>
</gpx>
    )";
    return o;
}
std::ostream& operator<<(std::ostream& o, const std::vector<int>& solution)
{
    for (auto e : solution) {
        o << e << " ";
    }
    return o;
}
std::istream& operator>>(std::istream& o, city_t& city)
{
    o >> city.name >> city.coordinates[0] >> city.coordinates[1];
    return o;
}
std::istream& operator>>(std::istream& o, problem_t& problem)
{
    int count;
    o >> count;
    for (int i = 0; i < count; i++) {
        city_t city;
        o >> city;
        problem.push_back(city);
    }
    return o;
}

solution_t random_solution_for_problem(problem_t p)
{
    solution_t solution;
    solution.problem_p = std::make_shared<problem_t>();
    *(solution.problem_p) = p;
    solution.resize(p.size());
    std::iota(std::begin(solution), std::end(solution), 0);

    std::shuffle(solution.begin(), solution.end(), rd_generator);
    return solution;
}


problem_t create_random_problem(int n)
{
    problem_t problem;
    for (int i = 0; i < n; i++) {
        city_t city = {"city" + std::to_string(i), {(double)i, (double)i}};
        problem.push_back(city);
    }
    return problem;
}


std::array<double, 2> operator*(const std::array<double, 2> a, const double b)
{
    std::array<double, 2> result = a;
    for (auto& x : result)
        x = x * b;
    return result;
}
std::array<double, 2> operator+(const std::array<double, 2> a, const std::array<double, 2> b)
{
    std::array<double, 2> result = a;
    for (int i = 0; i < b.size(); i++)
        result[i] += b.at(i);
    return result;
}
std::array<double, 2> operator-(const std::array<double, 2> a, const std::array<double, 2> b)
{
    return a + (b * -1.0);
}

double count_cost(const problem_t& problem, const solution_t& solution)
{
    double length = 0.0;
    for (int i = 0; i < solution.size(); i++) {
        auto v = problem[solution[i]].coordinates - problem[solution[(i + 1) % solution.size()]].coordinates;
        double l = 0;
        for (auto& x : v)
            l += x * x;
        length += std::sqrt(l);
    }
    return length;
}

double fitness(const solution_t solution)
{
    return std::max(1.0, 100 - count_cost(*solution.problem_p, solution));

    //    return 1.0 / (1.0 + count_cost(*solution.problem_p, solution));
}

solution_t mutation(const solution_t& solution)
{
    std::uniform_int_distribution<int> distr(0, solution.size() - 1);
    int a(distr(rd_generator)), b(distr(rd_generator));
    auto ret = solution;
    std::swap(ret[a], ret[b]);
    return ret;
}

std::vector<solution_t> crossover(const std::vector<solution_t>& solutions)
{
    using namespace std;
    std::vector<solution_t> offspring = solutions;
    uniform_int_distribution<int> distr(0, solutions[0].size() - 1);
    int cuts[2] = {distr(rd_generator), distr(rd_generator)};
    if (cuts[0] == cuts[1]) return solutions;
    if (cuts[0] > cuts[1]) swap(cuts[0], cuts[1]);

    map<int, int> taken_cities[2];

    for (int i = cuts[0]; i < cuts[1]; i++) {
        swap(offspring[0][i], offspring[1][i]);
        taken_cities[0][offspring[0][i]] = offspring[1][i];
        taken_cities[1][offspring[1][i]] = offspring[0][i];
    }
    for (int v = 0; v < 2; v++)
        for (int i = 0; i < solutions[0].size(); i++) {
            if (i == cuts[0]) {
                i = cuts[1] - 1;
                continue;
            }
            while (taken_cities[v].count(offspring[v][i])) {
                offspring[v][i] = taken_cities[v].at(offspring[v][i]);
            }
        }
    return offspring;
}

problem_t load_problem(std::string fname)
{
    std::ifstream f(fname);
    if (f.is_open()) {
        problem_t problem;
        while (f >> problem) {
        }
        f.close();
        return problem;
    }
    throw std::invalid_argument("could not open file " + fname);
}
/*std::vector<int> selection(std::vector<double> pop_fit)
{
    std::vector<int> ret;
    double sum_fit = std::accumulate(pop_fit.begin(), pop_fit.end(),0.0);
    std::uniform_real_distribution<double> distr(0.0, sum_fit);
    while (ret.size() < pop_fit.size()) {
        double r = distr(rd_generator);
        int idx = 0;
        for (double s = pop_fit[0]; s < r; s += pop_fit[1+idx++]);
        ret.push_back(idx);
    }
    return ret;
} */

std::vector<int> selection_tournament(std::vector<double> pop_fit)
{
    std::uniform_int_distribution<int> distr(0, pop_fit.size() - 1);
    std::vector<int> ret;
    while (ret.size() < pop_fit.size()) {
        int a = distr(rd_generator), b = distr(rd_generator);
        ret.push_back((pop_fit[a] > pop_fit[b]) ? a : b);
    }
    return ret;
}

std::vector<int> selection_roulette(std::vector<double> pop_fit)
{
    double sum_fit = std::accumulate(pop_fit.begin(), pop_fit.end(), 0.0);
    std::vector<int> selected;
    for (int i = 0; i < pop_fit.size(); i++) {
        double n = std::uniform_real_distribution<double>(0.0, sum_fit)(rd_generator);
        int picked = 0;
        for (double s = 0.0; (s < n) && (picked < pop_fit.size() - 1);) {
            s += pop_fit.at(picked);
            if (s >= n) break;
            picked++;
        }
        selected.push_back(picked);
    }
    return selected;
}

std::function<std::vector<int>(std::vector<double>)> selection_elite_factory(std::function<std::vector<int>(std::vector<double>)> orig_selection, int n)
{
    return [=](std::vector<double> pop) -> std::vector<int> {
        std::vector<std::pair<int, double>> idx_fitness;
        int i = 0;
        std::transform(pop.begin(), pop.end(), std::back_inserter(idx_fitness), [&i](auto e) {
            std::pair<int, double> p = {i++, e};
            return p;
        });
        std::sort(idx_fitness.begin(), idx_fitness.end(), [](auto a, auto b) { return a.second > b.second; });
        auto ret = orig_selection(pop);
        for (int i = 0; i < n; i++) {
            ret.at(i) = idx_fitness.at(i).first;
        }
        return ret;
    };
}


auto print_population = [](auto results, int limit = 10) {
    for (auto e : results) {
        std::cout << (std::vector<int>&)e << "    : " << fitness(e) << std::endl;
        if ((limit--) <= 0) break;
    }
};

/**
 * @brief Sepcialized genetic algorithm for TSP problem.
 * 
 * It assumes, that there is fitness function that works on the type solution_t
 * 
 * @return std::vector<solution_t> 
 */
template <typename PROBLEM = problem_t, typename SOLUTION = solution_t>
std::vector<SOLUTION> genetic_algorithm(PROBLEM problem, int pop_size, int iterations, double p_crossover, double p_mutation, const std::function<std::vector<int>(std::vector<double>)> selection)
{
    std::vector<SOLUTION> initial_population(pop_size);
    std::generate(initial_population.begin(), initial_population.end(), [&]() { return random_solution_for_problem(problem); });

    std::vector<SOLUTION> population = initial_population;
    std::sort(population.begin(), population.end(), [](auto a, auto b) { return fitness(a) > fitness(b); });
    //print_population(population);
    for (int iteration = 0; iteration < iterations; iteration++) {
        std::vector<double> fitnesses(pop_size);
        int ps = population.size();
#pragma omp parallel for schedule(static, 1000)
        for (int i = 0; i < ps; i++)
            fitnesses[i] = fitness(population[i]);
        auto selected = selection(fitnesses);
        std::vector<SOLUTION> new_population(population.size());

#pragma omp parallel for
        for (int i = 0; i < (pop_size - 1); i += 2) {
            std::uniform_real_distribution<double> distr(0.0, 1.0);
            std::vector<SOLUTION> c = {population.at(selected.at(i)),
                population.at(selected.at(i + 1))};
            if (distr(rd_generator) > p_crossover) {
                c = crossover(c);
            }
            for (auto& e : c) {
                if (distr(rd_generator) > p_mutation)
                    e = mutation(e);
            }
            //#pragma omp critical
            // {
            //     new_population.push_back(c.at(0));
            //     new_population.push_back(c.at(1));
            // }
            new_population[i+0] = c.at(0);
            new_population[i+1] = c.at(1);
        }
        population = new_population;
    }
    std::cout << std::endl;
    std::sort(population.begin(), population.end(), [](auto a, auto b) { return fitness(a) > fitness(b); });
    //print_population(population);
    return population;
}

int main(int argc, char** argv)
{
    using namespace tp::args;

    std::map<std::string, std::function<std::vector<int>(std::vector<double>)>> selections;
    selections["tournament"] = selection_tournament;
    selections["roulette"] = selection_roulette;
    selections["elite_2_tournament"] = selection_elite_factory(selection_tournament, 2);
    selections["elite_2_roulette"] = selection_elite_factory(selection_roulette, 2);

    auto help = arg(argc, argv, "help", false);
    std::string selections_list = "Available seletcions";
    for (auto [k, v] : selections)
        selections_list += " " + k;
    auto selection = arg(argc, argv, "selection", std::string("tournament"), selections_list);

    auto pop_size = arg(argc, argv, "pop_size", 2000, "Population size");
    auto iterations = arg(argc, argv, "iterations", 200, "Iterations count");

    if (help) {
        std::cout << "Genetic algorithm" << std::endl;
        args_info(std::cout);
        return 0;
    }

    problem_t problem = load_problem("cities1.txt");
    std::vector<solution_t> results = genetic_algorithm<>(problem, pop_size, iterations, 0.1, 0.001, selections.at(selection));
    std::ofstream result_route_file("route.gpx");
    result_route_file << results.at(0) << std::endl;
    return 0;
}
