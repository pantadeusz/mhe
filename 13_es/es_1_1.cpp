#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <random>
#include <vector>

std::random_device random_device_0;
std::default_random_engine random_generator_0(random_device_0());
auto norm_dist = []() {
    static std::normal_distribution<double> N(0.0, 1.0);
    return N(random_generator_0);
};
auto uni_dist = [](int min_, int max_) {
    std::uniform_int_distribution<int> N(min_, max_);
    return N(random_generator_0);
};
auto uni_double_dist = [](const double a = 0.0, const double b = 1.0) {
    static std::uniform_real_distribution<double> N(a, b);
    return N(random_generator_0);
};

using specimen_t = std::array<double, 2>;
/**
 * strategy - (1+1)
 * 
 * k - liczba iteracji branych pod uwagę w liczeniu 1/5 sukcesów
 * */
specimen_t evolution_strategy_1_1(specimen_t X0, std::function<double(specimen_t)> opt_fun, int iterations = 100, int k = 10)
{
    using namespace std;
    double delta = 2.0;
    
    vector<specimen_t> X = {X0};
    vector<specimen_t> Y;
    for (int t = 0; t < iterations; t++) {
        Y.push_back(X.back());
        for (auto &y:Y.back()) {
            y = y + delta*norm_dist();
        }

/// regula 1/5
        int all_tries = 0, successe = 0;
        for (int i = 0; (i < k) && (i < X.size()); i++) {
            all_tries++;
            successe += (opt_fun(Y[Y.size()-1-i]) > opt_fun(X[X.size()-1-i]))?1.0:0.0;
        }
        if ((successe*5) > all_tries) {
            delta = delta*(1.0/0.82);
        } else if ((successe*5) < all_tries) {
            delta = delta*0.82;
        }
        


        if (opt_fun(Y.back()) > opt_fun(X.back())) {
            X.push_back(Y.back()); // sukces
        } else {
            X.push_back(X.back());
        }

        cout << t << " " <<  successe << "/" << all_tries << " " << delta << " " << X.back()[0] << " " << X.back()[1] << " " << opt_fun(X.back()) << endl;
    } 
    return X.back();
}

auto sphere_f = [](auto v) -> double {
    double sum = 0;
    for (auto e : v) {
        sum += e * e;
    }
    return 1 / (1.0 + sum);
};

auto ackley = [](auto d) {
    double x = d.at(0);
    double y = d.at(1);
    return 10.0/(10.0+ ((-20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * cos(2 * M_PI * x) + cos(2 * M_PI * y)) + M_E + 20)));
};

auto himmelblau = [](auto d) {
    double x = d.at(0);
    double y = d.at(1);
    return 1.0/(1.0 + (pow(x * x + y - 11, 2.0) + pow(x + y * y - 7, 2)));
};

int main(int argc, char** argv)
{
    auto result = evolution_strategy_1_1({
                                             uni_double_dist(-10, 10),
                                             uni_double_dist(-10, 10),
                                         },
        ackley, 100);
    std::cout << result[0] << " " << result[1] << std::endl;
    return 0;
}
