

#include "problem_tsp.hpp"
#include "solver_brute_force.hpp"
#include "solver_ga.hpp"
#include "solver_hill_climb.hpp"
#include "solver_hill_climb_nd.hpp"
#include "solver_sa.hpp"
#include "solver_tabu.hpp"
#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <set>

#include "json.hpp"

int main() {

  using namespace std;
  tsp_solution_t problem;
  map<string, map<string, string>> results;
  random_device rd;
  mt19937 gen(rd());
  cin >> problem;

  {
    int pop_size = 100;
    double prob_crossover = 0.9;
    auto init_population = [pop_size, problem]() {
      vector<tsp_solution_t> pop;
      for (int i = 0; i < pop_size; i++) {
        pop.push_back(randomize_solution(problem));
      }
      return pop;
    };
    auto selection = [&gen](vector<tsp_solution_t> &pop, vector<double> fits) {
      uniform_int_distribution<> dis(0, pop.size() - 1);
      vector<tsp_solution_t> ret;
      for (int i = 0; i < pop.size(); i++) {
        auto ai = dis(gen);
        auto bi = dis(gen);
        if (fits[ai] > fits[bi])
          ret.push_back(pop[ai]);
        else
          ret.push_back(pop[bi]);
      }
      return ret;
    };
    auto crossover = [prob_crossover, &gen](vector<tsp_solution_t> &parents) {
      uniform_real_distribution<> dis(0.0, 1.0);
      uniform_int_distribution<> cross_distribution(
          0, parents.back().cities_to_see.size() - 1);

      vector<tsp_solution_t> offspring;
      for (unsigned i = 1; i < parents.size(); i++) {
        if (dis(gen) < prob_crossover) {
          auto p1 = parents[i];
          auto p2 = parents[i - 1];
          auto o1 = parents[i];
          auto o2 = parents[i - 1];

          set<int> o1used;
          set<int> o2used;

          int cross_point = cross_distribution(gen);
          for (int l = 0; l < cross_point; l++) {
            o1used.insert(o1.cities_to_see[l]);
            o2used.insert(o2.cities_to_see[l]);
          }
          for (int l = cross_point; l < o1.cities_to_see.size(); l++) {
            if (o1used.count(p2.cities_to_see[l]))
              o1.cities_to_see[l] = -1;
            else {
              o1.cities_to_see[l] = p2.cities_to_see[l];
              o1used.insert(p2.cities_to_see[l]);
            }

            if (o2used.count(p1.cities_to_see[l]))
              o2.cities_to_see[l] = -1;
            else {
              o2.cities_to_see[l] = p1.cities_to_see[l];
              o1used.insert(p1.cities_to_see[l]);
            }
          }
          list<int> o1free;
          list<int> o2free;
          for (int i = 0; i < o1.cities_to_see.size(); i++) {
            if (o1used.count(i) == 0)
              o1free.push_back(i);
            if (o2used.count(i) == 0)
              o2free.push_back(i);
          }
          for (int l = cross_point; l < o1.cities_to_see.size(); l++) {
            if (o1.cities_to_see[l] < 0) {
              o1.cities_to_see[l] = o1free.front();
              o1free.pop_front();
            }
            if (o2.cities_to_see[l] < 0) {
              o2.cities_to_see[l] = o2free.front();
              o2free.pop_front();
            }
          }
          offspring.push_back(o1);
          offspring.push_back(o2);
        } else {
          offspring.push_back(parents[i - 1]);
          offspring.push_back(parents[i]);
        }
      }
      return offspring;
    };
    // brak mutacji  !!!!!
    auto mutation = [](vector<tsp_solution_t> &parents) { return parents; };
    auto fitness = [](tsp_solution_t &o) { return 1000.0 / (goal(o) + 1.0); };
    int i = 0;
    // liczba iteracji
    auto term_condidtion = [&i](vector<tsp_solution_t> &pop,
                                vector<double> fits) { return i++ < 100; };
    auto t_start = chrono::steady_clock::now();
    auto pop = genetic_algorithm(init_population, selection,
                                 crossover, //
                                 mutation, fitness, term_condidtion);
    auto t_finish = chrono::steady_clock::now();
    auto best = *max_element(std::begin(pop), std::end(pop),
                     [](auto a, auto b) { return goal(a) > goal(b); });
    cout << "#genetic_algorithm GOAL: " << goal(best)
         << " dt: " << (t_finish - t_start).count() << endl;
    results["genetic_algorithm"] = {{"goal", to_string(goal(best))}};
  }
  {
    uniform_real_distribution<> dis(0.0, 1.0);
    auto get_random_neighbour = [&gen](auto e) {
      auto list0 = get_close_solutions(e);
      vector<tsp_solution_t> full_list;
      for (auto ne : list0) {
        auto n = get_close_solutions(ne);
        full_list.insert(full_list.end(), n.begin(), n.end());
      }
      for (int i = 0; i < 3; i++)
        full_list.insert(full_list.end(), list0.begin(), list0.end());
      uniform_int_distribution<> dis(0, full_list.size() - 1);
      return full_list.at(dis(gen));
    };
    auto u = [&dis, &gen](int n) { return dis(gen); };
    auto t_start = chrono::steady_clock::now();
    auto sol = simulated_annealing(
        [&]() { return randomize_solution(problem); }, get_random_neighbour,
        [](int n) { return 1.0 / n; }, goal, u, 2000);
    auto t_finish = chrono::steady_clock::now();
    cout << "#simulated_annealing GOAL: " << goal(sol)
         << " dt: " << (t_finish - t_start).count() << endl;
    results["simulated_annealing"] = {{"goal", to_string(goal(sol))}};
  }
  {
    auto t_start = chrono::steady_clock::now();
    auto sol = tabu_search([&]() { return randomize_solution(problem); },
                           get_close_solutions, goal, 2000, 50);
    auto t_finish = chrono::steady_clock::now();
    cout << "#tabu_search GOAL: " << goal(sol)
         << " dt: " << (t_finish - t_start).count() << endl;
    results["tabu_search"] = {{"goal", to_string(goal(sol))}};
  }
  {
    auto t_start = chrono::steady_clock::now();
    auto sol = hill_climbing([&]() { return randomize_solution(problem); },
                             get_close_solutions, goal, 2000);
    auto t_finish = chrono::steady_clock::now();
    cout << "#hill_climbing GOAL: " << goal(sol)
         << " dt: " << (t_finish - t_start).count() << endl;
    results["hill_climbing"] = {{"goal", to_string(goal(sol))}};
  }
  {
    auto t_start = chrono::steady_clock::now();
    auto sol = hill_climbing_nd([&]() { return randomize_solution(problem); },
                                get_close_solutions, goal, 2000);
    auto t_finish = chrono::steady_clock::now();
    cout << "#hill_climbing_nd GOAL: " << goal(sol)
         << " dt: " << (t_finish - t_start).count() << endl;
    results["hill_climbing_nd"] = {{"goal", to_string(goal(sol))}};
  }
  {
    auto t_start = chrono::steady_clock::now();
    auto sol =
        brute_force([problem]() { return problem; }, next_solution, goal);
    auto t_finish = chrono::steady_clock::now();
    cout << "#brute_force GOAL: " << goal(sol)
         << " dt: " << (t_finish - t_start).count() << endl;
    results["brute_force"] = {{"goal", to_string(goal(sol))}};
  }
  nlohmann::json r = results;
  cout << r.dump() << endl;
  return 0;
}