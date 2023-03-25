
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
/// zadanie do rozwiązania

class city_t {
public:
  double x, y;
  std::string name;
  city_t operator-(const city_t c) {
      return {x-c.x,y-c.y,name};
  };
  double len() {
      return std::sqrt(x*x+y*y);
  }
};
using problem_t = std::vector<city_t>;


class solution_t {
public:
  virtual double goal() const = 0;
};

class tsp_solution_t : public solution_t {
public:
  std::shared_ptr<problem_t> problem;
  std::vector<int> cities_to_see;
  double goal() const {
      double distance = 0.0;
      auto p = problem->at(cities_to_see.at(0));
      for (int i = 1; i <= problem->size(); i++) {
          distance += (p - problem->at(cities_to_see[i%problem->size()])).len();
          p = problem->at(cities_to_see[i%problem->size()]);
      }
      return distance;
  }
};

int main() {
    using namespace std;
    problem_t problem = {
        {1.0,1.0, "Lębork"},
        {5.0,1.0, "Kościerzyna"},
        {2.0,10.5, "Piekło Dolne"},
        {20.0,10.0, "Złe Mięso"}
    };

    tsp_solution_t sol;
    sol.problem = make_shared<problem_t>(problem);
    sol.cities_to_see = {0,1,2,3};
    cout << sol.goal() << endl;
    sol.cities_to_see = {1,2,0,3};
    cout << sol.goal() << endl;
    return 0;
}