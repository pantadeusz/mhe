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

// obiektowo-funkcyjny

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
    double distance()
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
        static random_device rd;
        static mt19937 r(rd());
        uniform_int_distribution<int> uni((unsigned long)0, size() - 1);
        Solution s = *this;
        auto a = uni(r);
        auto b = uni(r);
        auto tmp = s[a];
        s[a] = s[b];
        s[b] = tmp;
        return s;
    }

    void randomize()
    {
        using namespace std;
        static random_device rd;
        static mt19937 r(rd());
        // 3 1 4  0 2
        //
        shuffle(this->begin(), this->end(), r);
        /*
        vector<int> indexes;
        std::vector<int> result;
        for (int i = 0; i < size(); i++) indexes.push_back(i);
        for (auto e: indexes) cout << e << " ";
        cout << endl;
        for (int i = size()-1; i >= 0; i--) {
            uniform_int_distribution<int> uni((unsigned long)0, i);
            int p = uni(r);
            result.push_back(indexes[p]);
            auto tmp = indexes[p];
            indexes[p] = indexes[i];
            indexes[i] = tmp;
            for (auto e: indexes) cout << e << " ";
            cout << endl;
        }
        for (unsigned i = 0; i < size(); i++) {
            (*this)[i] = result[i];
        }
        */
        //for (auto e: *this) cout << e << " ";
        //cout << endl;
    }

    Solution add_axis(int ax) const
    {
        Solution nsol = *this;
        int prevCityIdx = nsol[ax];
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
    Solution sub_axis(int ax) const
    {
        Solution nsol = *this;
        int prevCityIdx = nsol[ax];
        nsol[ax] = (nsol[ax] + size() - 1) % size();
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
            break;
        is >> x >> y;
        p.push_back({name, {x, y}});
    } while (true);
    return is;
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

auto hill_climbing_rand = [](auto problem) {
    using namespace std;

    Solution sol(problem);
    sol.randomize();
    auto best = sol;
    for (int i = 0; i < 10000; i++)
    {
        sol = sol.generate_random_neighbour();
        if (sol.distance() < best.distance())
        {
            best = sol;
            cout << "+";
        }
        else
        {
            cout << ".";
        }
        // (można wykomentować aby nie zaśmiecało nam konsoli)  //cout << sol << endl;
    };
    cout << endl;
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

auto tabu = [](auto problem) {
    using namespace std;
    Solution sol(problem);
    sol.randomize();

    list<Solution> tabu_list;
    tabu_list.push_back(sol);
    for (int i = 0; i < 10000; i++)
    {
        vector<Solution> neighbours; // sasiedzie nie w tabu
        for (int i = 0; i < sol.size(); i++)
        {
            auto e1 = tabu_list.back().add_axis(i);
            auto e2 = tabu_list.back().sub_axis(i);
            bool found1 = false;
            bool found2 = false;
            for (auto &e : tabu_list) {
                if (e == e1) {found1 = true; break;}
            }
            for (auto &e : tabu_list) {
                if (e == e2) {found2 = true; break;}
            }
            if (!found1) neighbours.push_back(e1);
            if (!found2) neighbours.push_back(e2);
        }
        sort(neighbours.begin(), neighbours.end(), [](auto &a, auto &b) {
            return a.distance() > b.distance();
        });
        tabu_list.push_back(neighbours.back());
        if (sol.distance() > neighbours.back().distance()) sol = neighbours.back();
        if (tabu_list.size() > 40) tabu_list.pop_front(); // ograniczenie do 40
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
    if (argc > 1)
    {
        auto best = brute_force(problem);
        cout << "BEST(BF): " << best << endl;
    }
    {
        auto best = hill_climbing_rand(problem);
        cout << "BEST(HC): " << best << endl;
    }
    {
        auto best = tabu(problem);
        cout << "BEST(tabu): " << best << endl;
    }
    return 0;
}
