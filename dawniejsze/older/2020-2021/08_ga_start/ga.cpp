
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


string chromosome_to_string(chromosome_t c) {
    string s ="";
    for (auto i : c) s = s + to_string((int)i);
    return s;
}

int roulette_selection(const vector<chromosome_t> &pop, const vector<double> &pop_fit)
{
    double s = 0.0;
    for (int i = 0; i < pop.size(); i++) s += pop_fit[i];
    uniform_real_distribution<double> dist(0,s);
    double p_sum = 0.0;
    double r = dist(randgen);
//    cout << pop.size() << " s:" << s << " r:" << r ;
    for (int i = 0; i < pop.size(); i++) {
        p_sum += pop_fit[i];
//        cout << " [" << i << "]" << p_sum;
        if (r < p_sum) {
//            cout << endl;
            return i;
        }
    }
//    cout << " -> " << " s:" << s << " r:" << r << endl;
    throw std::invalid_argument("you cannot have r out of range");
}

vector<chromosome_t> one_point_crossover(double p_cross, const chromosome_t &a_, const chromosome_t &b_)
{
    uniform_int_distribution<int> dist(0, (int)a_.size()-1);
    int pp = dist(randgen);
    auto a = a_;
    auto b = b_;
    for( int l = pp; l < a.size(); l++) {
        swap(a[l],b[l]);
    }
//    cout << "["<< pp <<"]: " << chromosome_to_string(a_) << " -> " << chromosome_to_string(a) << endl;
//    cout << "["<< pp <<"]: " << chromosome_to_string(b_) << " -> " << chromosome_to_string(b) << endl;
    return {a, b};
}

chromosome_t uniform_mutation(double p_mut, chromosome_t c)
{
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
        cout << "[" << iteration << "]";
        for (int i = 0; i < pop.size(); i++) {
            cout << " " << pop_fit[i] << ":" << chromosome_to_string(pop[i]);
        }
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

/// implementacja zadania

double one_max_function(chromosome_t c)
{
    double sum = 0;
    for (auto g : c)
    {
        sum += g;
    }
    return sum;
}

/// program

int main()
{
    ga(10, 12, 10, one_max_function);
    return 0;
}
