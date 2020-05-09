
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "json.hpp"
// dot -Kfdp -n -Tpng example.dot  > example.png
using nlohmann::json;


tsp_solution_t brute_force(problem_t problem) {
  using namespace std;
  tsp_solution_t sol;
  sol.problem = make_shared<problem_t>(problem);
  for (int i = 0; i < sol.problem->size(); i++)
    sol.cities_to_see.push_back(i);
  auto first_solution = sol;
  auto best_solution = sol;
  do {
    sol.next_solution();
    if (sol.goal() < best_solution.goal()) {
      best_solution = sol;
    }
  } while (!(sol == first_solution));
  return best_solution;
}

tsp_solution_t hill_climbing(problem_t problem) {
  using namespace std;

  tsp_solution_t sol;
  sol.problem = make_shared<problem_t>(problem);

  sol.randomize_solution();

  auto first_solution = sol;
  do {
    // cout << "goal: " << sol.goal() << endl;
    // sol.next_solution();
    auto neighbours = sol.get_close_solutions();
    auto best_neighbour = neighbours.back(); 
    for (auto neighbour : neighbours) { // najlepsze rozwiazanie z otoczenia
      if (neighbour.goal() < best_neighbour.goal()) {
        best_neighbour = neighbour;
      }
    }

    if (sol.goal() > best_neighbour.goal()) {
        sol = best_neighbour;
    } else { /// warunek zakonczenia
      return sol;
    }
  } while (true);
}

tsp_solution_t hill_climbing_nd(problem_t problem) {
  using namespace std;
    using namespace std;
    static random_device rd;
    static mt19937 gen(rd());

  tsp_solution_t sol;
  sol.problem = make_shared<problem_t>(problem);

  sol.randomize_solution();

  auto first_solution = sol;
  for (int i = 0 ; i< 100; i++ ){
    auto neighbours = sol.get_close_solutions();
    uniform_int_distribution<> dist(0, neighbours.size() - 1);
    auto best_neighbour = neighbours.at(dist(gen)); 
    if (sol.goal() >= best_neighbour.goal()) {
        sol = best_neighbour;
        cout << "[" << i << "]goal: " << sol.goal() << endl;
    }
  };
  return sol;
}

int main() {

  using namespace std;
  using namespace nlohmann;
  problem_t problem; /* = {
       {1.0,1.0, "Lębork"},
       {5.0,1.0, "Kościerzyna"},
       {2.0,10.5, "Piekło Dolne"},
       {20.0,10.0, "Złe Mięso"}
   };*/

  cin >> problem;
  {
    auto sol = brute_force(problem);
    cout << 
    //sol << 
    "#brute_force GOAL: " << sol.goal() << endl;
  }
  {
    auto sol = hill_climbing(problem);
    cout << 
    //sol << 
    "#hill_climbing GOAL: " << sol.goal() << endl;
  }
  {
    auto sol = hill_climbing_nd(problem);
    cout << 
    //sol << 
    "#hill_climbing_nd GOAL: " << sol.goal() << endl;
  }
  return 0;
}