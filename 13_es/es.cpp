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
    std::vector<double> y;                        ///< wartosc rozwiazania
    std::vector<double> s;                        ///< odchylenia standardowe
    std::function<double(const specimen_t& s)> f; // funkcja do optymalizacji
    bool operator<(const specimen_t& j) const     // porownanie - wywolujemy funkcje oceny
    {
        const specimen_t& i = *this;
        return f(i) < f(j);
    }
} specimen_o;

specimen_t operator*(const specimen_t& x, double a)
{
    specimen_t ret = x;
    for (auto& e : ret.y)
        e *= a;
    for (auto& e : ret.s)
        e *= a;
    return ret;
};
specimen_t operator+(const specimen_t& x, const specimen_t& y)
{
    specimen_t ret = x;
    for (unsigned i = 0; i < x.y.size(); i++) {
        ret.y[i] += y.y[i];
        ret.s[i] += y.s[i];
    }
    return ret;
};
using population_t = std::vector<specimen_t>;
std::ostream& operator<<(std::ostream& o, const specimen_t& s)
{
//    o << "( ";
    for (auto v : s.y)
        o << v << " ";
//    o << ", ";
    for (auto s_ : s.s)
        o << s_ << " ";
    o << " " << s.f(s);
    return o;
}
/**
 * generates single specimen with vector of length n, and fitness function f
 *
 * @arg n specimen length
 * @arg f fitness function
 * */
specimen_t specimen_of(
    int n,
    std::function<double(const specimen_t& s)> f =
        [](auto&) -> double {
        throw std::invalid_argument("fitness not defined");
    })
{
    using namespace std;
    specimen_t ret = {vector<double>(n, 0.0), vector<double>(n, 1.0), f};
    return ret;
}

/**
 * strategy - (mi+lambda))
 * */
specimen_t evolution_strategy(int lambda, population_t init_pop, int iterations = 100)
{
    using namespace std;
    const int mu = init_pop.size();
    // const int rho = 2; // only defalut method is supported
    const double tau = 1.0 / sqrt(2.0 * init_pop.back().y.size());
    const double tau_prim = 1.0 / sqrt(2.0 * sqrt((double)init_pop.back().y.size()));
    const double n = init_pop.back().y.size();
    vector<population_t> P;
    P.push_back(init_pop);
    int g = 0; // first generation

    do {
        auto& P_g = P.back(); // current population
        population_t T;
        for (int i = 0; i < lambda; i += 2) {
            auto X_1 = P_g.at(uni_dist(0, P_g.size() - 1));
            auto X_2 = P_g.at(uni_dist(0, P_g.size() - 1));

            double a = uni_double_dist();

            auto X_1_p = X_1 * a + X_2 * (1.0 - a);
            auto X_2_p = X_2 * a + X_2 * (1.0 - a);
            double common_random_value = norm_dist();
            for (int i = 0; i < n; i++) {
                X_1_p.s[i] = X_1_p.s.at(i) * exp(tau_prim * common_random_value + tau * norm_dist());
                X_2_p.s[i] = X_2_p.s.at(i) * exp(tau_prim * common_random_value + tau * norm_dist());

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
        cout << g << " " << P.back().back() << " " << P.back().size() << endl;

        g++;
    } while (g < iterations);
    sort(P.back().begin(), P.back().end());
    return P.back().at(0);
}

auto sphere_f = [](auto v) -> double {
    double sum = 0;
    for (auto e : v.y) {
        sum += e * e;
    }
    return 1 / (1.0 + sum);
};

auto ackley = [](auto pp) {
    auto d = pp.y;
    double x = d.at(0);
    double y = d.at(1);
    return 100 - (-20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * cos(2 * M_PI * x) + cos(2 * M_PI * y)) + M_E + 20);
};

auto himmelblau = [](auto pp) {
    auto d = pp.y;
    double x = d.at(0);
    double y = d.at(1);
    return 1000 - (pow(x * x + y - 11, 2.0) + pow(x + y * y - 7, 2));
};

int main(int argc, char** argv)
{
    population_t initial_population;
    for (int i = 0; i < 20; i++) {
        initial_population.push_back(specimen_of(2, ackley));
    }
    for (auto& e : initial_population) {
        for (auto& y : e.y)
            y = norm_dist() * 3.0;
        for (auto& s : e.s)
            s = std::abs(norm_dist() * 1.0);
    }
    auto result = evolution_strategy(20, initial_population, 100);
    std::cout << "# " << result << std::endl;
    return 0;
}
