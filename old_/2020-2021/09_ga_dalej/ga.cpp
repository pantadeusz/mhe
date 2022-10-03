
#include <vector>
#include <random>
#include <functional>
#include <iostream>
#include <algorithm>
#include <numeric>

using chromosome_t = std::vector<char>;

using namespace std;

random_device rdevice;
mt19937 randgen(rdevice());

string chromosome_to_string(chromosome_t c)
{
    string s = "";
    for (auto i : c)
        s = s + to_string((int)i);
    return s;
}

int roulette_selection(const vector<chromosome_t> &pop, const vector<double> &pop_fit)
{
    double s = 0.0;
    for (int i = 0; i < pop.size(); i++)
        s += pop_fit[i];
    uniform_real_distribution<double> dist(0, s);
    double p_sum = 0.0;
    double r = dist(randgen);
    //    cout << pop.size() << " s:" << s << " r:" << r ;
    for (int i = 0; i < pop.size(); i++)
    {
        p_sum += pop_fit[i];
        if (r < p_sum)
        {
            //            cout << endl;
            return i;
        }
    }
    //    cout << " -> " << " s:" << s << " r:" << r << endl;
    throw std::invalid_argument("you cannot have r out of range");
}

int tournament_selection(const vector<chromosome_t> &pop, const vector<double> &pop_fit)
{
    int tournament_size = 2;
    uniform_int_distribution<int> dist(0, pop.size() - 1);
    vector<int> tournament;
    for (int t = 0; t < tournament_size; t++)
    {
        tournament.push_back(dist(randgen));
    }
    sort(
        tournament.begin(), tournament.end(), [&](auto a, auto b) { return pop_fit[a] > pop_fit[b]; });
    return tournament.at(0);
}

vector<chromosome_t> one_point_crossover(double p_cross, const chromosome_t &a_, const chromosome_t &b_)
{
    uniform_int_distribution<int> dist(0, (int)a_.size() - 1);
    int pp = dist(randgen);
    auto a = a_;
    auto b = b_;
    for (int l = pp; l < a.size(); l++)
    {
        swap(a[l], b[l]);
    }
    //    cout << "["<< pp <<"]: " << chromosome_to_string(a_) << " -> " << chromosome_to_string(a) << endl;
    //    cout << "["<< pp <<"]: " << chromosome_to_string(b_) << " -> " << chromosome_to_string(b) << endl;
    return {a, b};
}

vector<chromosome_t> two_point_crossover(double p_cross, const chromosome_t &a_, const chromosome_t &b_)
{
    uniform_int_distribution<int> dist(0, (int)a_.size() - 1);
    int pp1 = dist(randgen);
    int pp2 = dist(randgen);
    if (pp1 > pp2) swap(pp1,pp2);
    auto a = a_;
    auto b = b_;
    for (int l = pp1; l < pp2; l++)
    {
        swap(a[l], b[l]);
    }
    //    cout << "["<< pp <<"]: " << chromosome_to_string(a_) << " -> " << chromosome_to_string(a) << endl;
    //    cout << "["<< pp <<"]: " << chromosome_to_string(b_) << " -> " << chromosome_to_string(b) << endl;
    return {a, b};
}


chromosome_t uniform_mutation(double p_mut, chromosome_t c)
{
    static std::uniform_real_distribution<double> dist(0, 1.0);
    for (auto &g : c)
    {
        if (dist(randgen) < p_mut)
        {
            g = 1 - g;
        }
    }
    return c;
}

vector<chromosome_t> ga(int population_size, int chromosome_size, int iterations,
                        function<double(chromosome_t)> fitness_f,
                        double p_cross = 0.9, double p_mut = 0.1,
                        function<int(const vector<chromosome_t> &, const vector<double> &)> selection = roulette_selection,
                        function<vector<chromosome_t>(double, const chromosome_t &, const chromosome_t &)> crossover = one_point_crossover,
                        function<chromosome_t(double, const chromosome_t &)> mutation = uniform_mutation)
{
    uniform_int_distribution<char> distr(0, 1);
    vector<chromosome_t> pop(population_size);
    vector<double> pop_fit(population_size);

    // populacja początkowa
    for (auto &c : pop)
    {
        c.resize(chromosome_size);
        for (auto &g : c)
        {
            g = distr(randgen);
        }
    }

    for (int iteration = 0; iteration < iterations; iteration++)
    {
        // liczymy fitness dla wszystkich elementow
        for (unsigned i = 0; i < pop.size(); i++)
        {
            pop_fit[i] = fitness_f(pop[i]);
        }
        double avg_fit = std::accumulate(pop_fit.begin(), pop_fit.end(), 0.0) / (double)pop_fit.size();
        double best_fit = std::accumulate(pop_fit.begin(), pop_fit.end(), pop_fit[0], [](double a, double b) {
            return (a > b) ? a : b;
        });

        cout << iteration << " ";
        cout << p_cross << " " << avg_fit << " " << best_fit << " ";
        /*        for (int i = 0; i < pop.size(); i++)
        {
            cout << " " << pop_fit[i] << ":" << chromosome_to_string(pop[i]);
        } */
        cout << endl;

        vector<chromosome_t> parents;
        vector<chromosome_t> children;
        for (int i = 0; i < pop.size(); i++)
            parents.push_back(pop[selection(pop, pop_fit)]);
        int parents_idx = 1;
        while ((children.size() < pop.size()) && (parents_idx < pop.size()))
        {
            for (auto e : crossover(p_cross, parents.at(parents_idx), parents.at(parents_idx - 1)))
                children.push_back(e);
            parents_idx += 2;
        }

        for (auto &c : children)
        {
            c = mutation(p_mut, c);
        }

        pop = children;
    }
    return pop;
}

/// implementacja zadania - one max

double one_max_function(chromosome_t c)
{
    double sum = 0;
    for (auto g : c)
    {
        sum += g;
    }
    return sum;
}

/// implementacja zadania - ackley

auto ackley = [](std::pair<double, double> p) -> double {
    using namespace std;
    auto [x, y] = p;
    return -20.0 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * (cos(2 * M_PI * x) + cos(2 * M_PI * y))) + M_E + 20.0;
};

auto ackley_decode = [](chromosome_t genotype) -> std::pair<double, double> {
    double x = 0.0;
    double y = 0.0;
    for (int i = 0; i < genotype.size() / 2; i++)
        x = x * 2 + genotype[i];
    for (int i = genotype.size() / 2; i < genotype.size(); i++)
        y = y * 2 + genotype[i];

    x = x / pow(2.0, (genotype.size() / 2 - 4)) - 8;
    y = y / pow(2.0, (genotype.size() / 2 - 4)) - 8;
    return {x, y};
};

auto get_fitness_fun_for_cost_function = [](auto decode_f, auto cost_f,
                                            auto transform_f) {
    auto f = [=](chromosome_t genotype) {
        auto fenotype = decode_f(genotype);
        auto cost = cost_f(fenotype);
        // return 1.0;
        return transform_f(cost);
    };
    return f;
};

/// program

int main(int argc, char **argv)
{

    int pop_size = stoi(argv[1]);
    int chromosome_size = stoi(argv[2]);
    int iterations = stoi(argv[3]);
    double p_cross = stod(argv[4]);
    double p_mut = stod(argv[5]);

    auto fit = get_fitness_fun_for_cost_function(ackley_decode, ackley,
                                                 [](double cost) { return 1.0 / (1.0 + cost); });
    auto result_p = ga(pop_size, chromosome_size, iterations,
                       fit,
                       p_cross, p_mut,
                       tournament_selection,
                       two_point_crossover);
    auto result = std::max_element(result_p.begin(), result_p.end(), [&](auto a, auto b) {
        return fit(a) < fit(b);
    });
    cout << "# result: f( " << ackley_decode(*result).first << " " << ackley_decode(*result).second << " ) = "
         << ackley(ackley_decode(*result)) << endl;
    cout << "::" << ackley({0.0, 0.0});
    return 0;
}
