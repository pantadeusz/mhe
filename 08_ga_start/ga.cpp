
#include <vector>
#include <random>
#include <functional>

using chromosome_t = std::vector<char>;

using namespace std;

random_device rdevice;
mt19937 randgen(rdevice());

int roulette_selection(const vector<chromosome_t> &pop, const vector<double> &pop_fit)
{
    return 0;
}

vector<chromosome_t> one_point_crossover(double p_cross, const chromosome_t &a, const chromosome_t &b)
{
    return {a, b};
}

chromosome_t uniform_mutation(double p_mut, chromosome_t c) {
    return c;
}

vector<chromosome_t> ga(int max_pop, int chromosome_size, int iterations,
                        function<double(chromosome_t)> fitness_f,
                        double p_cross = 0.9, double p_mut = 0.1,
                        function<int(const vector<chromosome_t> &, const vector<double> &)> selection = roulette_selection,
                        function<vector<chromosome_t>(double, const chromosome_t &, const chromosome_t &)> crossover = one_point_crossover,
                        function<chromosome_t(double, const chromosome_t &)> mutation = uniform_mutation)
{
    uniform_int_distribution<char> distr(0, 1);
    vector<chromosome_t> pop(max_pop);
    vector<double> pop_fit(max_pop);

    // populacja początkowa
    for (auto &c : pop)
    {
        c.resize(chromosome_size);
        for (auto &g : c)
        {
            g = distr(randgen);
        }
    }
    // liczymy fitness dla wszystkich elementow
    for (unsigned i = 0; i < pop.size(); i++)
    {
        pop_fit[i] = fitness_f(pop[i]);
    }

    for (int iteration = 0; iteration < iterations; iteration++)
    {
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

        for (auto &c : children) {
            c = mutation(p_mut, c);
        }

        pop = children;
        // liczymy fitness dla wszystkich elementow
        for (unsigned i = 0; i < pop.size(); i++)
        {
            pop_fit[i] = fitness_f(pop[i]);
        }
    }
    return pop;
}

int main()
{

}
