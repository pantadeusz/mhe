#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <random>
#include <vector>

auto norm_dist = []() {
  static std::random_device r;
  static std::default_random_engine g(r());
  static std::normal_distribution<double> N(0.0, 1.0);
  return N(g);
};
auto uni_dist = [](int min_, int max_) {
  static std::random_device r;
  static std::default_random_engine g(r());
  std::uniform_int_distribution<int> N(min_, max_);
  return N(g);
};
auto uni_double_dist = []() {
  static std::random_device r;
  static std::default_random_engine g(r());
  static std::uniform_real_distribution<double> N(0.0, 1.0);
  return N(g);
};
// mu rho lambda f

struct specimen_t {
  std::vector<double> y;
  std::vector<double> s;
  std::function<double(const specimen_t &s)> f;
  bool operator<(const specimen_t &j) const {
    const specimen_t &i = *this;
    return (f(i) < f(j));
  }
} specimen_o;
specimen_t operator*(const specimen_t &x, double a) {
  specimen_t ret = x;
  for (auto &e : ret.y)
    e *= a;
  for (auto &e : ret.s)
    e *= a;
  return ret;
};
specimen_t operator+(const specimen_t &x, const specimen_t &y) {
  specimen_t ret = x;
  for (unsigned i = 0; i < x.y.size(); i++) {
    ret.y[i] += y.y[i];
    ret.s[i] += y.s[i];
  }
  return ret;
};
using population_t = std::vector<specimen_t>;
std::ostream &operator<<(std::ostream &o, const specimen_t &s) {
  o << "( ";
  for (auto v : s.y)
    o << v << " ";
  o << ", ";
  for (auto s_ : s.s)
    o << s_ << " ";
  o << ", " << s.f(s) << " )";
  return o;
}
/**
 * generates single specimen with vector of length n, and fitness function f
 *
 * @arg n specimen length
 * @arg f fitness function
 * */
specimen_t specimen_of(int n,
                       std::function<double(const specimen_t &s)> f =
                           [](auto &) -> double {
                         throw std::invalid_argument("fitness not defined");
                       }) {
  using namespace std;
  specimen_t ret = {vector<double>(n, 0.0), vector<double>(n, 1.0), f};
  return ret;
}

specimen_t evolution_strategy(int lambda, population_t init_pop, int iterations = 100) {
  using namespace std;
  const int mu = init_pop.size();
  // const int rho = 2; // only defalut method is supported
  const double tau = 1.0 / (sqrt(2.0 * init_pop.back().y.size()));
  const double n = init_pop.back().y.size();
  vector<population_t> P;
  P.push_back(init_pop);
  int g = 0; // first generation

  do {

    auto &P_g = P.back(); // current population
    population_t T;
    for (int i = 0; i < lambda; i += 2) {
      auto X_1 = P_g.at(uni_dist(0, P_g.size() - 1));
      auto X_2 = P_g.at(uni_dist(0, P_g.size() - 1));
      double a = uni_double_dist();

      auto X_1_p = X_1 * a + X_2 * (1.0 - a);
      auto X_2_p = X_2 * a + X_2 * (1.0 - a);

      for (int i = 0; i < n; i++) {

        X_1_p.s[i] = X_1_p.s.at(i) * exp(tau * norm_dist());
        X_2_p.s[i] = X_2_p.s.at(i) * exp(tau * norm_dist());

        X_1_p.y[i] = X_1_p.y.at(i) + X_1_p.s[i] * norm_dist();
        X_2_p.y[i] = X_2_p.y.at(i) + X_2_p.s[i] * norm_dist();
      }
      T.push_back(X_1_p);
      T.push_back(X_2_p);
    }

    population_t OP(T.begin(), T.end());
    OP.insert(OP.end(), P_g.begin(), P_g.end()); // for strategy "+"
    sort(OP.begin(), OP.end());

    P.push_back(population_t(OP.begin() + (OP.size() - mu), OP.end()));
    cout << "F: " << P.back().back().f(P.back().back()) << endl;

    g++;
  } while (g < iterations);
  sort(P.back().begin(), P.back().end());
  return P.back().at(0);
}

int main(int argc, char **argv) {
  auto fun_to_opt = [](auto v) -> double {
    auto x = v.y.at(0);
    auto y = v.y.at(1);
    if (std::abs(x) > 10.0)
      return 0.0;
    if (std::abs(y) > 10.0)
      return 0.0;
    auto ret = 1000000.0 / (1.0 + (0.26 * (x * x + y * y) - 0.48 * x * y));
    if (ret < 0.0)
      ret = 0;
    return ret;
  };
  population_t initial_population;
  for (int i = 0; i < 20; i++) {
    initial_population.push_back(specimen_of(2, fun_to_opt));
  }
  for (auto &e : initial_population) {
    for (auto &y : e.y)
      y = norm_dist() * 3.0;
    for (auto &s : e.s)
      s = std::abs(norm_dist() * 1.0);
  }
  auto result = evolution_strategy(20, initial_population, 100);
  std::cout << result << std::endl;
  return 0;
}
