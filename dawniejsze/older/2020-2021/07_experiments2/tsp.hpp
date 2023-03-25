#ifndef ___TSP_PROBLEM_IMPLEMENTATION___
#define ___TSP_PROBLEM_IMPLEMENTATION___

#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <memory>
#include <functional>
#include <random>

/* zadanie do rozwiązania */
struct City
{
    std::string name;        // nazwa miasta
    std::array<double, 2> x; // wsp. miasta
    /**
     * Oblicza odleglosc do miasta
     * */
    double dist(const City &c);
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
    std::shared_ptr<Problem> problem;

    Solution();
    Solution(std::shared_ptr<Problem> p);
    /**
     * odwołujemy się do miast z problemu do rozwiażania
     * */
    /**
     * metoda pozwalająca na bezpieczne ustawienie problemu do rozwiązania.
     * Inicjuje ona listę indeksów miast na kolejne wartości, czyli kolejność
     * miast do odwiedzenia jest taka jak w podanym zadaniu.
     * */
    void set_problem(std::shared_ptr<Problem> p);
    /**
     * metoda licząca długość trasy odwiedzającej wszystkie miasta.
     * To też jest nasza minimalizowana funkcja celu!!!
     * */
    double distance() const;

    /**
     * Oblicza kolejne możliwe rozwiązanie. Ta metoda ma taką cechę, że 
     * pozwala na deterministyczne przejście po wszystkich permutacjach.
     * Nadaje się więc do rozwiązania siłowego.
     * */
    Solution generate_next(std::function<void(void)> on_end = []() {}) const;
    /**
     * Generuje nam losowego sąsiada.
     * */
    Solution generate_random_neighbour(std::mt19937 &mt) const;

    void randomize(std::mt19937 &mt);

    // dodaje 1 na zadanej wspolrzednej
    Solution inc_axis(int ax) const;
    // odejmuje 1 na zadanej wspolrzednej
    Solution dec_axis(int ax) const;
};

/**
 * Wczytywanie problemu z strumienia wejściowego. Jeśli jako nazwę miasta poda się "-" to kończy wczytywać.
 * */
std::istream &operator>>(std::istream &is, Problem &p);
/**
 * Wypisanie rozwiązania na standardowe wyjście
 * */
std::ostream &operator<<(std::ostream &o, Solution &s);

bool operator==(const Solution &a, const Solution &b);

#endif
