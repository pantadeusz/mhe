

#include "problem_tsp.hpp"
#include "solver_brute_force.hpp"
#include "solver_hill_climb_nd.hpp"
#include "solver_hill_climb.hpp"
#include <iostream>
#include <map>

#include "json.hpp"

int main() {

  using namespace std;
  tsp_solution_t problem;
  map<string,map<string,string>> results;
  cin >> problem;
    {
      auto sol = hill_climbing([&]() { return randomize_solution(problem); },
                                get_close_solutions, goal, 100);
      cout <<
      "#hill_climbing GOAL: " << goal(sol) << endl;
      results["hill_climbing"] = {{"goal",to_string(goal(sol))}};
    }
  {
    auto sol = hill_climbing_nd([&]() { return randomize_solution(problem); },
                                get_close_solutions, goal, 100);
    cout <<
        "#hill_climbing_nd GOAL: " << goal(sol) << endl;
      results["hill_climbing_nd"] = {{"goal",to_string(goal(sol))}};
  }
  {
    auto sol =
        brute_force([problem]() { return problem; }, next_solution, goal);
    cout <<
        "#brute_force GOAL: " << goal(sol) << endl;
      results["brute_force"] = {{"goal",to_string(goal(sol))}};
  }
  nlohmann::json r = results;
  cout << r.dump() << endl;
  return 0;
}