#include "tsp.hpp"
#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <numeric>
#include <algorithm>

/* zadanie do rozwiązania */

/**
     * Oblicza odleglosc do miasta
     * */
double City::dist(const City &c)
{
    return std::sqrt((c.x[0] - x[0]) * (c.x[0] - x[0]) + (c.x[1] - x[1]) * (c.x[1] - x[1]));
}

Solution::Solution()
{
}
Solution::Solution(std::shared_ptr<Problem> p)
{
    set_problem(p);
}

/**
     * metoda pozwalająca na bezpieczne ustawienie problemu do rozwiązania.
     * Inicjuje ona listę indeksów miast na kolejne wartości, czyli kolejność
     * miast do odwiedzenia jest taka jak w podanym zadaniu.
     * */
void Solution::set_problem(std::shared_ptr<Problem> p)
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
double Solution::distance() const
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
Solution Solution::generate_next(std::function<void(void)> on_end) const
{
    Solution s = *this;
    if (!std::next_permutation(s.begin(), s.end()))
        on_end();
    return s;
}
/**
     * Generuje nam losowego sąsiada.
     * */
Solution Solution::generate_random_neighbour(std::mt19937 &mt) const
{
    using namespace std;
    uniform_int_distribution<int> uni((unsigned long)0, size() - 1);
    uniform_int_distribution<int> incdec(0, 1);
    Solution s = *this;
    //std::swap(s[uni(mt)], s[uni(mt)]);
    if (incdec(mt) == 0)
    {
        inc_axis(uni(mt));
    }
    else
    {
        dec_axis(uni(mt));
    }
    return s;
}

void Solution::randomize(std::mt19937 &mt)
{
    using namespace std;
    shuffle(this->begin(), this->end(), mt);
}

// dodaje 1 na zadanej wspolrzednej
Solution Solution::inc_axis(int ax) const
{
    Solution nsol = *this;
    int prevCityIdx = nsol.at(ax);
    nsol[ax] = (nsol[ax] + 1) % size();
    for (int i = 0; i < size(); i++)
    {
        if ((nsol[i] == nsol[ax]) && (i != ax))
        {
            nsol[i] = prevCityIdx;
            return nsol;
        }
    }
    return nsol;
}
// odejmuje 1 na zadanej wspolrzednej
Solution Solution::dec_axis(int ax) const
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
            return nsol;
        }
    }
    return nsol;
}

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

bool operator==(const Solution &a, const Solution &b)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}
