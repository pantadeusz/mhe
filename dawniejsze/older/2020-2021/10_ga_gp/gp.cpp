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

/**
 * program ewolucyjny
 * */
auto ep = [](
			  auto population,
			  int iterations,
			  selection_f_t select,
			  crossover_f_t crossover,
			  mutation_f_t mutation,
			  double p_crossover,
			  double p_mutation) {
	for (int iteration = 0; iteration < iterations; iteration++)
	{
		vector<chromosome_t> new_pop;
		for (int c = 0; c < population.size() / 2; c++)
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
};

int main()
{
	vector<pair<double, double>> cities_coordinates = {
		{8.414709848078965, 5.403023058681398},
		{9.092974268256818, -4.161468365471424},
		{1.4112000805986722, -9.899924966004454},
		{-7.5680249530792825, -6.536436208636119},
		{-9.589242746631385, 2.8366218546322624},
		{-2.7941549819892586, 9.601702866503661},
		{6.569865987187891, 7.539022543433046},
		{9.893582466233818, -1.4550003380861354},
		{4.121184852417566, -9.11130261884677},
		{0, 10},
		{-5.440211108893697, -8.390715290764524},
		{-9.999902065507035, 0.044256979880507856},
		{-5.365729180004349, 8.438539587324922},
		{4.201670368266409, 9.074467814501961},
		{9.906073556948703, 1.367372182078336},
		{6.502878401571168, -7.596879128588213},
		{-2.879033166650653, -9.576594803233846},
		{-9.613974918795568, -2.7516333805159694},
		{-7.509872467716762, 6.603167082440802},
		{1.4987720966295235, 9.887046181866692},
		{9.129452507276277, 4.080820618133919},
		{8.36655638536056, -5.4772926022426836},
		{-0.08851309290403876, -9.99960826394637},
		{-8.462204041751706, -5.328330203333975},
		{-9.055783620066238, 4.24179007336997},
		{-1.3235175009777302, 9.912028118634735},
		{7.625584504796028, 6.469193223286403},
		{9.56375928404503, -2.9213880873383617},
		{2.7090578830786907, -9.626058663135666},
		{-6.636338842129676, -7.480575296890004}};

	auto init_pop = [](int s, int n) -> vector<chromosome_t> {
		vector<chromosome_t> ret;
		chromosome_t numbers;
		for (int i = 0; i < s; i++)
		{
			chromosome_t c;
			for (int i = 0; i < n; i++)
				numbers.push_back(i);
			for (int j = 0; j < n; j++)
			{
				int x = uniform_int_distribution<int>(0, n - 1 - j)(randgen);
				c.push_back(numbers[x]);
				numbers.erase(numbers.begin() + x);
			}
			ret.push_back(c);
		}
		return ret;
	};

	/// funkcja celu - trasa do przebycia lacznie z powrotem do pierwszego punktu
	auto goal = [=](chromosome_t c) {
		double l = 0;
		for (int i = 0; i < cities_coordinates.size(); i++)
		{
			auto [x1, y1] = cities_coordinates[c[i]];
			auto [x2, y2] = cities_coordinates[c[(i + 1) % cities_coordinates.size()]];
			l += sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		}
		return l;
	};
	/// funkcja fitness - przeksztalcona funkcja celu
	auto fitness = [=](chromosome_t c) {
		auto l = goal(c);
		return 1.0 / (1 + l);
	};

	// mechanizm selekcji - turniej rozmiaru 2
	selection_f_t select = [&](vector<chromosome_t> &population) -> chromosome_t {
		uniform_int_distribution<int> dist(0, population.size() - 1);
		auto s1 = population.at(dist(randgen));
		auto s2 = population.at(dist(randgen));
		if (fitness(s1) > fitness(s2))
			return s1;
		return s2;
	};

	crossover_f_t corssover_ox = [&](chromosome_t &p1, chromosome_t &p2, double p_crossover) -> pair<chromosome_t, chromosome_t> {
		uniform_int_distribution<int> dist(0, p1.size() - 1);
		int cpoint1 = dist(randgen);
		int cpoint2 = dist(randgen);
		if (cpoint2 < cpoint1)
			swap(cpoint1, cpoint2);

		// to jest generowanie jednego z potomkow, aby uzyskac pare, nalezy wywolac to dwa razy
		// z odpowiednio zamieniona kolejnoscia rodzicow
		auto swap_part = [cpoint1, cpoint2](chromosome_t p1, chromosome_t p2) -> chromosome_t {
			// kasujemy z rodzica numer 2 indeksy z fragmentu ciecia
			for (int i = cpoint1; i < cpoint2; i++)
				p2.erase(std::remove(p2.begin(), p2.end(), p1.at(i)), p2.end());
			// wstawiamy wycinany fragment na odpowiednie miejsce do potomka (ogonek sie odpowiednio przesunie)
			p2.insert(p2.begin() + cpoint1, p1.begin() + cpoint1, p1.begin() + cpoint2);
			// zwracamy potomka
			return p2;
		};

		return {swap_part(p1, p2), swap_part(p2, p1)};
	};

	// mutacja poprzez zamiane
	auto mutation_swap = [](chromosome_t c, double pm) -> chromosome_t {
		static uniform_real_distribution<double> dist(0.0, 1.0);
		if (dist(randgen) < pm)
		{
			uniform_int_distribution<int> cpointrnd(0, c.size() - 1);
			std::swap(c[cpointrnd(randgen)], c[cpointrnd(randgen)]);
			return c;
		}
		else
		{
			return c;
		}
	};

	/// wypisanie wynikow w sposob dobry do analizy
	auto print_stats = [&](auto comment, auto pop, bool print_for_chart = false) {
		/// wybranie wyniku
		auto best_of_all = *std::max_element(pop.begin(), pop.end(), [&](auto &a, auto &b) {
			return fitness(a) < fitness(b);
		});
		auto worse_of_all = *std::max_element(pop.begin(), pop.end(), [&](auto &a, auto &b) {
			return fitness(a) > fitness(b);
		});
		cout << "# [" << comment << "] best: " << fitness(best_of_all) << "   goal: " << goal(best_of_all) << endl;
		cout << "# [" << comment << "] worse: " << fitness(worse_of_all) << "   goal: " << goal(worse_of_all) << endl;
		/// tu mozna zrobic z tego wykres w gnuplocie
		if (print_for_chart)
		{
			double d = 0;
			auto p = cities_coordinates[best_of_all.at(0)];
			for (auto e : best_of_all)
			{
				auto r = cities_coordinates[e];
				d += sqrt(pow(p.first - r.first, 2.0) + pow(p.second - r.second, 2.0));
				cout << r.first << " " << cities_coordinates[e].second << " " << d << endl;
				p = r;
			}
		}
	};

	/// uruchomienie programu ewolucyjnego
	auto initial_population = init_pop(cities_coordinates.size() * 2, cities_coordinates.size());
	print_stats("initial", initial_population, false);
	auto result_population = ep(initial_population, 1000, select, corssover_ox, mutation_swap, 0.8, 0.1);
	print_stats("result", result_population, false);
	/// ile powinno wyjsc - okolo, poniewaz miasta nie pokrywaja wszystkich punktow kola, wiec wynik faktyczny powinien byc troche mniejszy
	std::cout
		<< "# Result should be somewhere near " << (2 * M_PI * 10.0) << std::endl;

	return 0;
}
