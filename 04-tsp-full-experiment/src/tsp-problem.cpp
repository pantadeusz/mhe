

#include "problem_tsp.hpp"
#include "solver_brute_force.hpp"
#include "solver_hill_climb.hpp"
#include "solver_hill_climb_nd.hpp"
#include "solver_sa.hpp"
#include "solver_tabu.hpp"
#include <chrono>
#include <iostream>
#include <map>
#include <random>

#include "json.hpp"

int main() {

  using namespace std;
  tsp_solution_t problem;
  map<string, map<string, string>> results;
  random_device rd;
  mt19937 gen(rd());
  cin >> problem;

  {
    uniform_real_distribution<> dis(0.0, 1.0);
    auto get_random_neighbour = [&gen](auto e) {
      auto list0 = get_close_solutions(e);
      vector<tsp_solution_t> full_list;
      for (auto ne : list0) {
        auto n = get_close_solutions(ne);
        full_list.insert(full_list.end(),n.begin(), n.end());
      }
      for (int i = 0; i < 3; i++) full_list.insert(full_list.end(),list0.begin(), list0.end());
      uniform_int_distribution<> dis(0, full_list.size()-1);
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