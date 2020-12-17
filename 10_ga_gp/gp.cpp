#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <functional>

using namespace std;

using chromosome_t = vector<int>;
using selection_f_t = function<chromosome_t(vector<chromosome_t> &)>;
using crossover_f_t = function<pair<chromosome_t, chromosome_t>(chromosome_t &, chromosome_t &, double)>;
using mutation_f_t = function<chromosome_t(chromosome_t, double)>;

random_device rd;
mt19937 randgen(rd());

vector<chromosome_t> init_pop(int s, int n)
{
	uniform_int_distribution<int> dist(0, n - 1);
	vector<chromosome_t> ret;
	for (int i = 0; i < s; i++)
	{
		chromosome_t c;
		for (int j = 0; j < n; j++)
		{
			c.push_back(dist(randgen));
		}
		ret.push_back(c);
	}
	return ret;
}

vector<chromosome_t> ep(int pop_size, int cities_count, int iterations,
				selection_f_t select,
				crossover_f_t crossover,
				mutation_f_t mutation,
				double p_crossover,
				double p_mutation)
{
	auto population = init_pop(pop_size, cities_count);
	for (int iteration = 0; iteration < iterations; iteration++)
	{
		vector<chromosome_t> new_pop;
		for (int c = 0; c < pop_size / 2; c++)
		{
			chromosome_t parent1 = select(population);
			chromosome_t parent2 = select(population);
			auto [child1, child2] = crossover(parent1, parent2, p_crossover);
			child1 = mutation(child1, p_mutation);
			child2 = mutation(child2, p_mutation);
			new_pop.push_back(child1);
			new_pop.push_back(child2);
		}
		population = new_pop;
	}
	return population;
}



int main()
{
	vector<pair<double,double>> cities_coordinates = {
{0,10},
{8.414709848078965,5.403023058681398},
{9.092974268256818,-4.161468365471424},
{1.4112000805986722,-9.899924966004454},
{-7.5680249530792825,-6.536436208636119},
{-9.589242746631385,2.8366218546322624},
{-2.7941549819892586,9.601702866503661},
{6.569865987187891,7.539022543433046},
{9.893582466233818,-1.4550003380861354},
{4.121184852417566,-9.11130261884677},
{-5.440211108893697,-8.390715290764524},
{-9.999902065507035,0.044256979880507856},
{-5.365729180004349,8.438539587324922},
{4.201670368266409,9.074467814501961},
{9.906073556948703,1.367372182078336},
{6.502878401571168,-7.596879128588213},
{-2.879033166650653,-9.576594803233846},
{-9.613974918795568,-2.7516333805159694},
{-7.509872467716762,6.603167082440802},
{1.4987720966295235,9.887046181866692},
{9.129452507276277,4.080820618133919},
{8.36655638536056,-5.4772926022426836},
{-0.08851309290403876,-9.99960826394637},
{-8.462204041751706,-5.328330203333975},
{-9.055783620066238,4.24179007336997},
{-1.3235175009777302,9.912028118634735},
{7.625584504796028,6.469193223286403},
{9.56375928404503,-2.9213880873383617},
{2.7090578830786907,-9.626058663135666},
{-6.636338842129676,-7.480575296890004}
	};

	auto fitness = [=](chromosome_t c) {
		double l = 0;
		for (int i = 0; i < cities_coordinates.size(); i++) {
			auto [x1,y1] = cities_coordinates[c[i]];
			auto [x2,y2] = cities_coordinates[c[(i+1)%cities_coordinates.size()]];
			l+= sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
		}
		return 1.0 / (1 + l);
	};

	selection_f_t select = [&](vector<chromosome_t> &population) -> chromosome_t {
		uniform_int_distribution<int> dist(0, population.size()-1);
		auto s1 = population.at(dist(randgen));
		auto s2 = population.at(dist(randgen));
		if (fitness(s1) > fitness(s2)) return s1;
		return s2;
	};

	crossover_f_t corssover_ox = [&](chromosome_t &p1, chromosome_t &p2, double p_crossover) -> pair<chromosome_t, chromosome_t> {
		uniform_int_distribution<int> dist(0, p1.size()-1);
		int cpoint1 = dist(randgen);
		int cpoint2 = dist(randgen);
		if (cpoint2 < cpoint1) swap(cpoint1,cpoint2);

		chromosome_t o1 = p1;
		chromosome_t o2 = p2;

		chromosome_t t1 = p2; 		
		chromosome_t t2 = p1; 		


		/// TODO - dokończyć
		for (int j = cpoint2; j < cpoint1 || j < cpoint1; j++) {
			for (int i = 0; i < t1.size(); i++) {
				if (t1[i] == o1[j]) {
					t1.erase (t1.begin(),t1.begin()+i);
					break;
				}
			}
		}

	};

	return 0;
}
