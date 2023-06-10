#include "tp_args.hpp"
#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "solution_t.h"
#include <tuple>
//std::random_device rd;

std::mt19937 rgen(109908093657865);


using namespace mhe;


solution_t brute_force(solution_t start_point)
{
    auto solution = start_point;
    for (int i = 0; i < solution.size(); i++) {
        solution[i] = i;
    }
    auto best_solution = solution;
    int i = 0;
    do {
        if (solution.goal() <= best_solution.goal()) {
            best_solution = solution;
            std::cout << (i++) << " " << solution << "  " << solution.goal() << " *** " << best_solution << "  "
                      << best_solution.goal() << std::endl;
        }
    } while (std::next_permutation(solution.begin(), solution.end()));
    return best_solution;
}

solution_t random_hillclimb(solution_t solution)
{
    for (int i = 0; i < 5040; i++) {
        auto new_solution = solution.random_modify(rgen);
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            std::cout << i << " " << solution << "  " << solution.goal() << std::endl;
        }
    }
    return solution;
}

solution_t deterministic_hillclimb(solution_t solution)
{
    for (int i = 0; i < 5040; i++) {
        auto new_solution = solution.best_neighbour();
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            std::cout << i << " " << solution << "  " << solution.goal() << std::endl;
        }
    }
    return solution;
}

solution_t tabu_search(solution_t solution)
{
    std::list<solution_t> tabu_list;
    std::set<solution_t> tabu_set;

    tabu_list.push_back(solution);
    tabu_set.insert(solution);

    solution_t best_globally = solution;
    for (int i = 0; i < 5040; i++) {
        auto neighbours = tabu_list.back().generate_neighbours();

        neighbours.erase(std::remove_if(neighbours.begin(),
                             neighbours.end(),
                             [&](solution_t x) { return tabu_set.contains(x); }),
            neighbours.end());
        if (neighbours.size() == 0) {
            std::cout << "Ate my tail..." << std::endl;
            return best_globally;
        }
        auto next_solution = *std::max_element(neighbours.begin(), neighbours.end(), [](auto l, auto r) {
            return l.goal() > r.goal();
        });

        if (next_solution.goal() <= best_globally.goal()) {
            best_globally = next_solution;
            std::cout << i << " " << best_globally << "  " << best_globally.goal() << std::endl;
        }
        tabu_list.push_back(next_solution);
        tabu_set.insert(next_solution);
    }
    return best_globally;
}

solution_t sim_annealing(const solution_t solution, std::function<double(int)> T)
{
    auto best_solution = solution; ///< globally best
    auto s = solution;             ///< current solution

    for (int i = 1; i < 5040; i++) {
        auto new_solution = s.random_modify(rgen);
        if (new_solution.goal() <= s.goal()) {
            s = new_solution;
            if (new_solution.goal() <= best_solution.goal()) {
                best_solution = s;
                std::cout << "*";
            }
            std::cout << i << " " << s << "  " << s.goal() << std::endl;
        } else {
            std::uniform_real_distribution<double> u(0.0, 1.0);
            if (u(rgen) < std::exp(-std::abs(new_solution.goal() - s.goal()) / T(i))) {
                s = new_solution;
            }
        }
    }
    return best_solution;
}

template <class T>
class genetic_algorithm_config_t
{
public:
    int population_size;
    virtual bool termination_condition(std::vector<solution_t>, std::vector<double>& fitnesses) = 0;
    virtual std::vector<T> get_initial_population() = 0;
    virtual double fitness(T) = 0;
    virtual std::vector<T> selection(std::vector<double>, std::vector<T>, std::mt19937& rgen) = 0;
    virtual std::vector<T> crossover(std::vector<T>, std::mt19937& rgen) = 0;
    virtual std::vector<T> mutation(std::vector<T>, std::mt19937& rgen) = 0;
};


class tsp_config_t : public genetic_algorithm_config_t<solution_t>
{
public:
    int iteration;
    int max_iterations;
    problem_t problem;

    double p_crossover;
    double p_mutation;
    tsp_config_t(int iter, int pop_size, double p_crossover_, double p_mutation_, problem_t problem_, std::mt19937& rgen)
    {
        max_iterations = iter;
        iteration = 0;
        population_size = pop_size;
        problem = problem_;
        p_mutation = p_mutation_;
        p_crossover = p_crossover_;
    }
    virtual bool termination_condition(std::vector<solution_t>, std::vector<double>& fitnesses)
    {
        iteration++;
        return iteration <= max_iterations;
    }

    virtual std::vector<solution_t> get_initial_population()
    {
        std::vector<solution_t> ret;
        for (int i = 0; i < population_size; i++) {
            ret.push_back(solution_t::random_solution(problem, rgen));
        }
        return ret;
    };


    virtual double fitness(solution_t solution)
    {
        return 1.0 / (1 + solution.goal());
    };

    virtual std::vector<solution_t> selection(std::vector<double> fitnesses, std::vector<solution_t> population, std::mt19937& rgen)
    {
        std::vector<solution_t> ret;
        while (ret.size() < population.size()) {
            std::uniform_int_distribution<int> dist(0, population.size() - 1);
            int a_idx = dist(rgen);
            int b_idx = dist(rgen);
            if (fitnesses[a_idx] >= fitnesses[b_idx])
                ret.push_back(population[a_idx]);
            else
                ret.push_back(population[b_idx]);
        }
        return ret;
    }

    std::pair<solution_t, solution_t> crossover(const std::pair<solution_t, solution_t>& solutions, std::mt19937& rd_generator)
    {
        using namespace std;
        std::vector<solution_t> offspring = {solutions.first, solutions.second};
        uniform_int_distribution<int> distr(0, solutions.first.size() - 1);
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
            for (int i = 0; i < offspring[0].size(); i++) {
                if (i == cuts[0]) {
                    i = cuts[1] - 1;
                    continue;
                }
                while (taken_cities[v].count(offspring[v][i])) {
                    offspring[v][i] = taken_cities[v].at(offspring[v][i]);
                }
            }
        return {offspring[0], offspring[1]};
    }

    virtual std::vector<solution_t> crossover(std::vector<solution_t> pop, std::mt19937& rgen)
    {
        std::vector<solution_t> offspring;
        for (int i = 0; i < pop.size(); i += 2) {
            std::uniform_real_distribution<double> distr(0.0, 1.0);
            if (distr(rgen) > p_mutation) {
                auto [a, b] = crossover(std::make_pair(pop.at(i), pop.at(i + 1)), rgen);
                offspring.push_back(a);
                offspring.push_back(b);
            } else {
                offspring.push_back(pop.at(i));
                offspring.push_back(pop.at(i + 1));
            }
        }
        return offspring;
    };
    virtual std::vector<solution_t> mutation(std::vector<solution_t> sol, std::mt19937& rgen)
    {
        std::vector<solution_t> ret(sol.size());
        std::transform(sol.begin(), sol.end(), ret.begin(), [&](auto e) {
            std::uniform_real_distribution<double> distr(0.0, 1.0);
            if (distr(rgen) > p_mutation)
                return e.random_modify(rgen);
            else
                return e;
        });
        return ret;
    };
};


template <class T>
solution_t generic_algorithm(genetic_algorithm_config_t<T>& cfg, int conv_curve, std::mt19937& rgen)
{
    auto population = cfg.get_initial_population();
    std::vector<double> fitnesses;
    int iteration = 0;
    for (int i = 0; i < population.size(); i++)
        fitnesses.push_back(cfg.fitness(population[i]));
    while (cfg.termination_condition(population, fitnesses)) {
        auto parents = cfg.selection(fitnesses, population, rgen);
        auto offspring = cfg.crossover(parents, rgen);
        offspring = cfg.mutation(offspring, rgen);
        population = offspring;
        fitnesses.clear();
        for (int i = 0; i < population.size(); i++)
            fitnesses.push_back(cfg.fitness(population[i]));
        if (conv_curve > 0) {
            if ((iteration % conv_curve) == 0) {
                double average = std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0) / fitnesses.size();
                std::cout << iteration << " " << average << std::endl;
            }
        }
        iteration++;
    }
    return *std::max_element(population.begin(), population.end(), [&](T l, T r) { return cfg.fitness(l) > cfg.fitness(r); });
}


solution_t shortest_distance(solution_t solution)
{
    solution_t result = solution;
    auto& problem = *solution.problem;
    solution.erase(solution.begin());
    for (int i = 1; i < result.size(); i++) {
        auto found = std::min_element(solution.begin(), solution.end(), [&](auto a, auto b) {
            double dist_a = len(problem[result[i - 1]] - problem[a]);
            double dist_b = len(problem[result[i - 1]] - problem[b]);
            return dist_a < dist_b;
        });
        result[i] = *found;
        solution.erase(found);
    }
    return result;
}

std::ostream& print_solution_for_graphviz(std::ostream& o, const solution_t v)
{
    auto pow_modulo = [](unsigned int a, unsigned int b, unsigned int mod) {
        unsigned int result = 1;
        for (int i = 1; i < b; i++)
            result = (result * b) % mod;
        return result;
    };
    o << "digraph {" << std::endl;
    o << "graph [pad=\"0.212,0.055\" bgcolor=lightgray]" << std::endl;
    o << "node [style=filled]" << std::endl;
    for (auto e : v) {
        auto x = v.problem->at(e)[0];
        auto y = v.problem->at(e)[1];
        o << "v_" << e << " [pos=\"" << x << "," << y << "!\" fillcolor=\"#" << std::hex << std::setfill('0') << std::setw(6)
          << (pow_modulo(7, e + 10, 0x0ffffff + 2) % 0x0ffffff) << "\"]" << std::endl;
    }
    for (int i = 0; i < v.size(); i++)
        o << "v_" << v[i] << " -> "
          << "v_" << v[(i + 1) % v.size()] << std::endl;
    o << "}" << std::endl;
    return o;
}


int main(int argc, char** argv)
{
    using namespace std;

    using namespace tp::args;
    auto help = arg(argc, argv, "help", false, "help screen");

    auto print_dot = arg(argc, argv, "print_dot", false, "show graphviz graph");
    auto conv_curve = arg(argc, argv, "conv_curve", 0, "how often show data to convergence curve");

    auto problem_size = arg(argc, argv, "problem_size", 30, "the number of cities");
    auto iterations = arg(argc, argv, "iterations", 1000, "iterations count");
    auto p_crossover = arg(argc, argv, "p_crossover", 0.1, "crossover probability");
    auto p_mutation = arg(argc, argv, "p_mutation", 0.1, "mutation probability");
    if (help) {
        std::cout << "help screen.." << std::endl;
        args_info(std::cout);
        return 0;
    }

    problem_t tsp_problem = generate_problem(problem_size, 10,
        10, rgen); //{{1.3, 1}, {2.4, 1}, {1.5, 2}, {3.1, 1}, {3.2, 7}, {3.3, 9}, {1.4, 4}};
    auto solution = solution_t::random_solution(tsp_problem, rgen);
    std::cout << tsp_problem << std::endl;
    std::cout << solution << "Start:  " << solution.goal() << std::endl;
    //solution = random_hillclimb(solution);
    //solution = brute_force(solution);
    //solution = shortest_distance(solution);
    //solution = deterministic_hillclimb(solution);
    //solution = tabu_search(solution);
    //solution = sim_annealing(solution, [](int k){return 1000.0/k;});
    tsp_config_t config(iterations, 5000, p_mutation, p_crossover, tsp_problem, rgen);
    solution = generic_algorithm<solution_t>(config, conv_curve, rgen);

    //*  { 6 0 3 5 2 1 4 }  30.9289 --  19.3343 // { 6 5 4 3 1 0 2 }  18.1745 -- bruteforce */
    std::cout << solution << "Result  " << solution.goal() << std::endl;
    if (print_dot) print_solution_for_graphviz(std::cout, solution.start_from_zero());
    return 0;
}
