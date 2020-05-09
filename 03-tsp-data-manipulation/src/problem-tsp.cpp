#include "problem-tsp.hpp"

class city_t {
public:
  double x, y;
  std::string name;
  city_t operator-(const city_t c) { return {x - c.x, y - c.y, name}; };
  double len() { return std::sqrt(x * x + y * y); }
};
using problem_t = std::vector<city_t>;

class solution_t {
public:
  virtual double goal() const = 0;
  virtual void next_solution() = 0;
};

class tsp_solution_t : public solution_t {
public:
  std::shared_ptr<problem_t> problem;
  std::vector<int> cities_to_see;
  double goal() const {
    double distance = 0.0;
    auto p = problem->at(cities_to_see.at(0));
    for (int i = 1; i <= problem->size(); i++) {
      distance += (p - problem->at(cities_to_see[i % problem->size()])).len();
      p = problem->at(cities_to_see[i % problem->size()]);
    }
    return distance;
  }

  void next_solution() {
    std::next_permutation(cities_to_see.begin(), cities_to_see.end());
  };
  
  void randomize_solution() {
    using namespace std;
    static random_device rd;
    static mt19937 gen(rd());
    vector<int> cities;
    for (int i = 0; i < problem->size(); i++)
      cities.push_back(i); // {0, 1, 2, 3 ...}
    cities_to_see.clear();

    for (int i = 0; i < problem->size(); i++) {
      uniform_int_distribution<> dist(0, cities.size() - 1);
      int r = dist(gen);
      int randomly_selected = cities.at(r);
      cities.erase(cities.begin() + r);
      cities_to_see.push_back(randomly_selected); // {0, 1, 2, 3 ...}
    }
  };
  std::vector<tsp_solution_t> get_close_solutions() {
    std::vector<tsp_solution_t> ret;
    for (unsigned i = 0; i < cities_to_see.size(); i++) {
      ret.push_back(*this);
    }

    for (int i=0; i < (int)cities_to_see.size(); i++) {
      std::swap(ret.at(i).cities_to_see.at(i),
                ret.at(i).cities_to_see.at((i+1) % cities_to_see.size())); 
    }
    return ret;
  }

/*
 (x1, x2, x3 ... xn)

 (x2, x1, x3, ... xn)
 (x1, x3, x2, ... xn)
 ...
*/
/*

 (x1, x2, x3 ... xn)

 (x1+1, x2, x3, ... , xn)
 (x1-1, x2, x3, ... , xn)
 (x1, x2+1, x3, ... , xn)
 (x1, x2-1, x3, ... , xn)
 (x1, x2, x3+1, ... , xn)
 (x1, x2, x3-1, ... , xn)
...
 (x1, x2, x3, ... , xn+1)
 (x1, x2, x3, ... , xn-1)

*/
};

bool operator==(const tsp_solution_t &a, const tsp_solution_t &b) {
  if (a.problem != b.problem)
    return false;
  if (a.cities_to_see.size() != b.cities_to_see.size())
    return false;
  for (unsigned i = 0; i < a.cities_to_see.size(); i++) {
    if (a.cities_to_see[i] != b.cities_to_see[i])
      return false;
  }
  return true;
}

std::istream &operator>>(std::istream &os, problem_t &problem) {
  json data;
  os >> data;
  for (auto e : data.at("problem")) {
    problem.push_back({x : e.at("x"), y : e.at("y"), name : e.at("name")});
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const tsp_solution_t &solution) {
  for (auto e : solution.cities_to_see) {
    os << solution.problem->at(e).x << " " << solution.problem->at(e).y
       << solution.problem->at(e).name << std::endl;
  }
  return os;
}
