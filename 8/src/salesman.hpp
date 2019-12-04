#ifndef __SALESMAN_MHE_HPP___
#define __SALESMAN_MHE_HPP___

#include <algorithm>
#include <cmath>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <random>

/**
 * We need some randomness. First random device (hopefully hardware)
 * */
static std::random_device seedsource;
/**
 * Then create default random engine
 * */
static std::default_random_engine generator(seedsource());


/**
 * @brief class representing single city on the map
 */
class city_t {
public:
  std::string name;
  double latitude;
  double longitude;

  /**
   * @brief calculate distance between two cities
   *
   * @param c2 second city to calculate distance to
   * @return double distance on the planet Earth
   */
  double distance(city_t &c2) {
    using namespace std;
    auto &[name2, lat2, lon2] = c2;
    auto R = 6371e3; // metres
    auto fi1 = latitude * M_PI / 180.0;
    auto fi2 = lat2 * M_PI / 180.0;
    auto deltafi = (lat2 - latitude) * M_PI / 180.0;
    auto deltalambda = (lon2 - longitude) * M_PI / 180.0;
    auto a = sin(deltafi / 2) * sin(deltafi / 2) +
             cos(fi1) * cos(fi2) * sin(deltalambda / 2) * sin(deltalambda / 2);
    auto c = 2 * atan2(sqrt(a), sqrt(1 - a));
    auto d = R * c;
    return d;
  }
};

/**
 * @brief the problem definition
 *
 * The cities we would like to see.
 */
class problem_t {
public:
  /**
   * @brief list of the cities to visit.
   */
  std::vector<city_t> cities;
};

/**
 * @brief the solution type
 *
 */
class solution_t {
public:
  /**
   * @brief smart pointer that have reference to the problem
   */
  std::shared_ptr<problem_t> problem;

  /**
   * @brief cities indices.
   * The order in wich we would like to see every city
   */
  std::vector<int> cities_to_see;

  /**
   * @brief Construct a new solution_t object
   *
   * This constructor will also generate new problem as a shared pointer so you
   * don't have to init the problem earlier.
   *
   * @param input_cities the cities list.
   */
  solution_t(const std::vector<city_t> input_cities)
      : problem(std::make_shared<problem_t>(problem_t{input_cities})),
        cities_to_see(input_cities.size()) {
    for (unsigned i = 0; i < cities_to_see.size(); i++)
      cities_to_see[i] = i;
  };

  /**
   * @brief Construct a new empty solution_t object
   *
   * @param input_cities the cities list.
   */
  solution_t()
      : problem(std::make_shared<problem_t>(problem_t{std::vector<city_t>()})),
        cities_to_see(0){};
  /**
   * @brief Construct a new solution t object
   * It takes shared pointer to the problem definition. This way we don't have
   * to copy the problem, only smartpointer to it.
   * @param problem_ the problem smartpointer
   */
  solution_t(std::shared_ptr<problem_t> problem_)
      : problem(problem_), cities_to_see(problem_->cities.size()) {
    for (unsigned i = 0; i < cities_to_see.size(); i++)
      cities_to_see[i] = i;
  };

  /**
   * @brief goal function value.
   *
   * It calculates the summary distance of the path following the solution
   * cities order. The traveller must come back to home.
   *
   * @return double lenght of the path.
   */
  double goal() const {
    double sum = 0.0;
    auto prev_city =
        cities_to_see.back(); // we must come back, so include the last city
    for (auto city : cities_to_see) { // now walk between cities
      sum += problem->cities[city].distance(problem->cities[prev_city]);
      prev_city = city;
    }
    return sum;
  };

  /**
   * @brief generates next possible solution
   *
   * @return problem_t the new solution to the problem
   */
  solution_t next_solution() const {
    auto p = *this;
    std::next_permutation(p.cities_to_see.begin() + 1, p.cities_to_see.end());
    return p;
  };

  /**
   * @brief This operator allows for checking if the solutions are the same and
   * the problem is the same
   *
   * @param other
   * @return true
   * @return false
   */
  bool operator==(const solution_t &other) const {
    if (this->problem !=
        other.problem) // is the problem the same (pointers the same)
      return false;
    if (other.cities_to_see.size() !=
        cities_to_see.size()) // do we have same number of cities
      throw std::invalid_argument(
          "cities count must be the same - problem in solution! check "
          "initializers and data consistency");
    for (unsigned i = 0; i < cities_to_see.size(); i++) { // check the cities order
      if (other.cities_to_see[i] != cities_to_see[i])
        return false;
    }
    return true;
  }
};

//// Reprezentacja porzadkowa
/**
 * This is the alternative solution type. It allows for easier enforcement of
 * correct solutions. On each index there is index of the city in temporary
 * "cities index" list. After processing each city index, the city is removed
 * form cities index list.
 * */
class alternative_solution_t {
public:
  std::vector<int>
      solution; ///< the solution -- the last value must be 0. Always!
  std::shared_ptr<problem_t> problem;

  /**
   * create alternative solution for given problem
   * */
  static alternative_solution_t of(const std::shared_ptr<problem_t> problem_) {
    return {std::vector<int>(problem_->cities.size()), problem_};
  }
  /**
   * create alternative solution for given simple solution
   * */
  static alternative_solution_t of(const solution_t &solution_) {
    return of(solution_.problem).set_solution(solution_);
  }
  /**
   * Create random alternative solution for given problem
   * */
  template <class RNG>
  static alternative_solution_t of(std::shared_ptr<problem_t> problem_,
                                   RNG &generator) {
    std::vector<int> solution(problem_->cities.size());
    for (unsigned i = 0; i < solution.size(); i++) {
      solution[i] = std::uniform_int_distribution<int>(0, solution.size() - 1 -
                                                              i)(generator);
    }
    return alternative_solution_t{solution, problem_};
  }

  /**
   * returns the original solution for current alternative solution
   * representation
   * */
  solution_t get_solution() const {
    using namespace std;
    solution_t ret(problem);
    vector<int> indexes(solution.size());
    for (unsigned i = 0; i < solution.size(); i++)
      indexes[i] = i;
    for (unsigned i = 0; i < solution.size(); i++) {
      ret.cities_to_see[i] = indexes[solution[i]];
      indexes.erase(indexes.begin() + solution[i]); // remove i-th element
    }
    return ret;
  }

  /**
   * Generate alternative solution representation for given solution.
   *
   * @return returns current alternative_solution_t
   * */
  alternative_solution_t &set_solution(solution_t s) {
    using namespace std;
    auto solc = s.cities_to_see;
    std::list<int> indices_list;
    solution.clear();
    int idx = solc.size();
    while (idx) {
      auto b = solc[--idx]; // current city number
      auto const insertion_point =
          std::upper_bound(indices_list.begin(), indices_list.end(), b);
      indices_list.insert(insertion_point, b);
      solution.push_back(std::distance(indices_list.begin(), insertion_point));
    }
    std::reverse(solution.begin(), solution.end());
    return *this;
  }

  template <class RNG>
  alternative_solution_t generate_random_neighbour(const int n_change,
                                                   RNG &generator) const {
    using namespace std;
    auto solution_candidate = *this;
    const auto n = solution_candidate.solution.size();
    uniform_int_distribution<int> distribution(0, n - 2);
    uniform_int_distribution<int> dist01(0, 1);
    for (int i = 0; i < n_change; i++) {
      int idx = distribution(generator); // select index numner to change
      // remember, we have max values defined like [n-1, n-2, ... 3, 2, 1]
      solution_candidate.solution[idx] =
          (solution_candidate.solution[idx] + (2 * dist01(generator) - 1) + n - idx) % (n - idx);
    }
    return solution_candidate;
  }

  double goal() const {
    return get_solution().goal();
  }

};

#endif