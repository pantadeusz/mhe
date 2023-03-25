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

#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>
#include <array>
#include <memory>
#include <functional>

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

/**
 * Funkcja main która implementuje aktualnie rozwiązanie metodą pełnego przeglądu.
 * */
int main()
{
    using namespace std;
    vector<vector<char>> field(3, vector<char>(4));

    auto problem = make_shared<Problem>();
    Solution sol;
    cin >> (*problem);
    sol.set_problem(problem);
    cout << sol << endl;
    auto best = sol;
    bool cont = true;
    do
    {
        sol = sol.generate_next([&]() { cont = false; });
        if (sol.distance() < best.distance())
            best = sol;
        // (można wykomentować aby nie zaśmiecało nam konsoli)  //cout << sol << endl;
    } while (cont);
    cout << "BEST: " << best << endl;
    return 0;
}
