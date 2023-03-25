/*
TSP z powrotem do punktu startowego

Zadanie do rozwiązania:
{(x0,y0),(x1,y1),...,(xn-1,yn-1)}

(dziedzina) Punkt roboczy, potencjalne rozwiązanie
(i0,i1,i2,...,in-1) -- lista kolejnych miast do odwiedzienia

Konstrukcja funkcji celu
Suma(odległość(miast))

Metoda modyfikacji rozwiązania
 -*kolejna permutacja
 - losowanie kolejnych poprawnych miast do odwiedzienia
 -*losowa zamiana 2 miast miejscami w kolejności
 - modyfikujemy jedno miasto i poprawa kolejnych miast tak aby żadne się nie powtórzyło
*/

/*
Generowanie nowych danych przykladowych: 

for k in `seq 1 100`; do rm input_$k.txt; for i in `seq 1 $k`; do echo "miasto${i} $RANDOM $RANDOM" >> input_$k.txt; done; echo "-" >> input_$k.txt; done

Testowanie czasow wykonania
for k in `seq 1 13`; do cat input_$k.txt | time ./a.out; done

*/

#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>
#include <array>
#include <memory>
#include <functional>
#include <set>
#include <list>
#include <map>
#include <chrono>
// obiektowo-funkcyjny

std::random_device rd;
std::mt19937 r_mt19937(rd());

/* zadanie do rozwiązania */
struct City
{
    std::string name;        // nazwa miasta
    std::array<double, 2> x; // wsp. miasta
    /**
     * Oblicza odleglosc do miasta
     * */
    double dist(const City &c)
    {
        return std::sqrt((c.x[0] - x[0]) * (c.x[0] - x[0]) + (c.x[1] - x[1]) * (c.x[1] - x[1]));
    }
};

/**
 * problem komiwojazera - zadanie
 * */
class Problem : public std::vector<City>
{
};

/**
 * lista kolejnych indeksów miast do odwiedzenia. Punkt roboczy, albo potencjalne rozwiązanie. Jest 
 * to zgodne z formatem wyjściowym rozwiązania.
 * */
class Solution : public std::vector<int>
{
public:
    Solution()
    {
    }
    Solution(std::shared_ptr<Problem> p)
    {
        set_problem(p);
    }
    /**
     * odwołujemy się do miast z problemu do rozwiażania
     * */
    std::shared_ptr<Problem> problem;
    /**
     * metoda pozwalająca na bezpieczne ustawienie problemu do rozwiązania.
     * Inicjuje ona listę indeksów miast na kolejne wartości, czyli kolejność
     * miast do odwiedzenia jest taka jak w podanym zadaniu.
     * */
    void set_problem(std::shared_ptr<Problem> p)
    {
        problem = p;
        this->resize(p->size());
        for (int i = 0; i < size(); i++)
            (*this)[i] = i;
    }
    /**
     * metoda licząca długość trasy odwiedzającej wszystkie miasta.
     * To też jest nasza minimalizowana funkcja celu!!!
     * */
    double distance() const
    {
        double sum = 0.0;
        for (unsigned i = 0; i < size(); i++)
        {
            sum += problem->at(at(i)).dist(problem->at(at((i + 1) % size())));
        }
        return sum;
    }

    /**
     * Oblicza kolejne możliwe rozwiązanie. Ta metoda ma taką cechę, że 
     * pozwala na deterministyczne przejście po wszystkich permutacjach.
     * Nadaje się więc do rozwiązania siłowego.
     * */
    Solution generate_next(std::function<void(void)> on_end = []() {}) const
    {
        Solution s = *this;
        if (!std::next_permutation(s.begin(), s.end()))
            on_end();
        return s;
    }
    /**
     * Generuje nam losowego sąsiada.
     * */
    Solution generate_random_neighbour() const
    {
        using namespace std;
        uniform_int_distribution<int> uni((unsigned long)0, size() - 1);
        Solution s = *this;
        std::swap(s[uni(r_mt19937)], s[uni(r_mt19937)]);
        return s;
    }

    void randomize()
    {
        using namespace std;
        shuffle(this->begin(), this->end(), r_mt19937);
    }

    // dodaje 1 na zadanej wspolrzednej
    Solution inc_axis(int ax) const
    {
        Solution nsol = *this;
        int prevCityIdx = nsol.at(ax);
        nsol[ax] = (nsol[ax] + 1) % size();
        for (int i = 0; i < size(); i++)
        {
            if ((nsol[i] == nsol[ax]) && (i != ax))
            {
                nsol[i] = prevCityIdx;
                break;
            }
        }
        return nsol;
    }
    // odejmuje 1 na zadanej wspolrzednej
    Solution dec_axis(int ax) const
    {
        Solution nsol = *this;
        int prevCityIdx = nsol[ax];
        nsol[ax] = (nsol[ax] + nsol.size() - 1) % size();
        for (int i = size() - 1; i >= 0; i--)
        {
            // zmiana
            if ((nsol[i] == nsol[ax]) && (i != ax))
            {
                nsol[i] = prevCityIdx;
                break;
            }
        }
        return nsol;
    }
};

/**
 * Wczytywanie problemu z strumienia wejściowego. Jeśli jako nazwę miasta poda się "-" to kończy wczytywać.
 * */
std::istream &operator>>(std::istream &is, Problem &p)
{
    using namespace std;
    do
    {
        string name;
        double x, y;
        is >> name;
        if (name == "-")
            return is;
        is >> x >> y;
        p.push_back({name, {x, y}});
    } while (true);
}

/**
 * Wypisanie rozwiązania na standardowe wyjście
 * */
std::ostream &operator<<(std::ostream &o, Solution &s)
{
    o << "[" << s.distance() << "] ";
    for (int i : s)
    {
        auto c = s.problem->at(i);
        o << c.name << "(" << c.x[0] << "," << c.x[1] << ") ";
    }
    return o;
}

auto brute_force = [](auto problem) {
    Solution sol(problem);
    auto best = sol;
    bool cont = true;
    do
    {
        sol = sol.generate_next([&]() { cont = false; });
        if (sol.distance() < best.distance())
            best = sol;
        // (można wykomentować aby nie zaśmiecało nam konsoli)  //cout << sol << endl;
    } while (cont);
    return best;
};

auto hill_climbing_rand = [](auto problem, int iterations = 1000) {
    using namespace std;

    Solution best(problem);
    best.randomize();
    for (int i = 0; i < iterations; i++)
    {
        auto sol = best.generate_random_neighbour();
        if (sol.distance() < best.distance())
        {
            cout << "found better: " << sol.distance() << endl;
            best = sol;
        }
    };
    return best;
};

auto hill_climbing_det = [](auto problem, int iterations = 1000) {
    using namespace std;
    Solution best(problem);
    best.randomize();
    for (int i = 0; i < iterations; i++)
    {
        vector<Solution> neighbours;
        neighbours.push_back(best);
        for (int a = 0; a < best.size(); a++)
        {
            neighbours.push_back(best.inc_axis(a));
            neighbours.push_back(best.dec_axis(a));
        }
        sort(neighbours.begin(), neighbours.end(), [](auto &a, auto &b) {
            return a.distance() > b.distance();
        });
        if (best == neighbours.back())
        {
            vector<Solution> best_neighbours;

            if (best.distance() == neighbours.at(neighbours.size() - 2).distance())
            {
                for (int i = neighbours.size() - 1; (i >= 0) && (best.distance() == neighbours.at(i).distance()); i--)
                    best_neighbours.push_back(neighbours[i]);
                using namespace std;

                uniform_int_distribution<int> uni((unsigned long)0, best_neighbours.size() - 1);
                best = best_neighbours.at(uni(r_mt19937));
            }
            else
            {
                break;
            }
        }
        best = neighbours.back();
    };
    return best;
};

bool operator==(const Solution &a, const Solution &b)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

auto tabu = [](auto problem, int iterations = 100000, int tabusize = 500000) {
    using namespace std;
    Solution sol(problem);
    sol.randomize();

    list<Solution> tabu_list;
    set<Solution> tabu_set;
    tabu_list.push_back(sol);
    tabu_set.insert(sol);
    for (int i = 0; i < iterations; i++)
    {
        vector<Solution> neighbours; // sasiedzi nie w tabu
        for (int i = 0; i < sol.size(); i++)
        {
            auto e1 = tabu_list.back().inc_axis(i);
            auto e2 = tabu_list.back().dec_axis(i);
            //if (std::find(tabu_list.begin(), tabu_list.end(), e1) == std::end(tabu_list))
            //    neighbours.push_back(e1);
            //if (std::find(tabu_list.begin(), tabu_list.end(), e2) == std::end(tabu_list))
            //    neighbours.push_back(e2);
            if (tabu_set.count(e1) == 0)
                neighbours.push_back(e1);
            if (tabu_set.count(e2) == 0)
                neighbours.push_back(e2);
        }
        if (neighbours.size() == 0)
        {
            neighbours.push_back(tabu_list.front()); // try to fix the problem of the snake eating its tail
        }
        sort(neighbours.begin(), neighbours.end(), [](auto &a, auto &b) {
            return a.distance() > b.distance();
        });
        tabu_list.push_back(neighbours.back());
        tabu_set.insert(neighbours.back());
        if (neighbours.back().distance() > neighbours[0].distance())
            throw std::invalid_argument("not a great list");
        if (sol.distance() > neighbours.back().distance())
        {
            sol = neighbours.back();
            cout << "found better: " << sol.distance() << endl;
        }
        if (tabu_list.size() > tabusize)
        {
            tabu_set.erase(tabu_list.front());
            tabu_list.pop_front(); // ograniczenie 
        }
    };
    return sol;
};

/**
 * Funkcja main która implementuje aktualnie rozwiązanie metodą pełnego przeglądu.
 * */
int main(int argc, char **argv)
{
    using namespace std;
    auto problem = make_shared<Problem>();
    cin >> (*problem);
    Solution sol(problem);
    map<string, string> args;
    string argname = "-m";
    args[argname] = "tabu";
    for (auto arg : vector<string>(argv + 1, argv + argc))
    {
        if (arg.size() && arg[0] == '-')
            argname = arg;
        else
            args[argname] = arg;
    }

    if (args["-m"] == "hill_climb")
    {
        auto start = std::chrono::steady_clock::now();
        auto best = hill_climbing_rand(problem, (args.count("-n") ? stoi(args["-n"]) : 1000));
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "hill_climb: " << elapsed_seconds.count() << " " << best << endl;
    }

    if (args["-m"] == "hill_climb_det")
    {
        auto start = std::chrono::steady_clock::now();
        auto best = hill_climbing_det(problem, (args.count("-n") ? stoi(args["-n"]) : 1000));
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "hill_climb_det: " << elapsed_seconds.count() << " " << best << endl;
    }
    if (args["-m"] == "tabu")
    {
        auto start = std::chrono::steady_clock::now();
        auto best = tabu(problem, (args.count("-n") ? stoi(args["-n"]) : 10000), (args.count("-tabusize") ? stoi(args["-tabusize"]) : 1000));
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "tabu: " << elapsed_seconds.count() << " " << best << endl;
    }
    if (args["-m"] == "full")
    {
        auto start = std::chrono::steady_clock::now();
        auto best = brute_force(problem);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "full: " << elapsed_seconds.count() << " " << best << endl;
    }
    return 0;
}
