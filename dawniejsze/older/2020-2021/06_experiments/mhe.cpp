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
#include "tsp.hpp"
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
#include <numeric>
// obiektowo-funkcyjny

std::random_device rd;
std::mt19937 r_mt19937(rd());

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
    list<Solution> history;
    Solution best(problem);
    best.randomize(r_mt19937);
    history.push_back(best);
    for (int i = 0; i < iterations; i++)
    {
        auto sol = best.generate_random_neighbour(r_mt19937);
        if (sol.distance() < best.distance())
        {
            //cout << "found better: " << sol.distance() << endl;
            best = sol;
        }
        history.push_back(best);
    };
    return history;
    //    return best;
};

auto hill_climbing_det = [](auto problem, int iterations = 1000) {
    using namespace std;
    Solution best(problem);
    best.randomize(r_mt19937);

    list<Solution> history;
    history.push_back(best);
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
        history.push_back(best);
    };
    //return best;
    return history;
};

auto tabu = [](auto problem, int iterations = 100000, int tabusize = 500000) {
    using namespace std;
    list<Solution> history;
    Solution sol(problem);
    sol.randomize(r_mt19937);

    list<Solution> tabu_list; // this is to keep the order of tabu elements
    set<Solution> tabu_set;   // this is to quickly check if the element is in tabu
    tabu_list.push_back(sol);
    tabu_set.insert(sol);
    for (int i = 0; i < iterations; i++)
    {
        vector<Solution> neighbours; // sasiedzi nie w tabu
        for (int i = 0; i < sol.size(); i++)
        {
            auto e1 = tabu_list.back().inc_axis(i);
            auto e2 = tabu_list.back().dec_axis(i);
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
            //cout << "found better: " << sol.distance() << endl;
        }
        if (tabu_list.size() > tabusize)
        {
            tabu_set.erase(tabu_list.front());
            tabu_list.pop_front(); // ograniczenie
        }
        history.push_back(tabu_list.back());
    };
    // return sol;
    return history;
};

auto sim_annealing = [](
                         auto problem, int iterations = 100000, auto T = [](int k) { return 0.1; }) {
    using namespace std;

    Solution s0(problem);
    s0.randomize(r_mt19937);
    list<Solution> s = {s0};

    for (int k = 0; k < iterations; k++)
    {
        auto tk = s.back().generate_random_neighbour(r_mt19937);
        if (tk.distance() <= s.back().distance())
        {
            s.push_back(tk);
        }
        else
        {
            uniform_real_distribution<double> ur(0, 1);
            double ep = exp(-abs(tk.distance() - s.back().distance()) / T(k));
            cerr << "ep: " << ep << endl;
            if (ur(r_mt19937) < ep)
            {
                s.push_back(tk);
            }
            else
            {
                s.push_back(s.back());
            }
        }
    }
    return s;
};

auto experiment_run = [](auto method_f, std::map<std::string, std::string> args) {
    using namespace std;
    auto start = std::chrono::steady_clock::now();
    //auto best = hill_climbing_rand(problem, (args.count("-n") ? stoi(args["-n"]) : 1000));
    auto ret_list = method_f(args);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    //cout << "hill_climb: " << elapsed_seconds.count() << " " << best << endl;
    int i = 0;
    for (auto elem : ret_list)
    {
        cout << (i++) << " " << elem.distance() << endl;
    }
    Solution best = *max_element(ret_list.begin(), ret_list.end(), [](auto a, auto b) { return a.distance() > b.distance(); });
    cout << "# " << args.at("-m") << ": " << elapsed_seconds.count() << " " << best << endl;
};
/**
 * Funkcja main która implementuje aktualnie rozwiązanie metodą pełnego przeglądu.
 * */
int main(int argc, char **argv)
{
    using namespace std;
    auto problem = make_shared<Problem>();
    cin >> (*problem);        // zaladujmy problem do rozwiazania
    Solution sol(problem);    // przygotujmy kandydata na rozwiazanie na podstawie problemu.
    map<string, string> args; // przetwarzanie argumentów - stworzy nam mapę [nazwaargumentu,wartoscargumentu] - wygodnie obsłużymy sobie argumenty
    string argname = "-m";
    args[argname] = "tabu";
    for (auto arg : vector<string>(argv + 1, argv + argc)) // pomijamy argument 0 - jest to nazawa programu
    {
        if (arg.size() && arg[0] == '-')
            argname = arg;
        else
            args[argname] = arg;
    }

    if (args["-m"] == "hill_climb")
    {
        experiment_run([&](auto args) {
            return hill_climbing_rand(problem, (args.count("-n") ? stoi(args["-n"]) : 1000));
        },
                       args);
    }
    if (args["-m"] == "hill_climb_det")
    {
        experiment_run([&](auto args) {
            return hill_climbing_det(problem, (args.count("-n") ? stoi(args["-n"]) : 1000));
        },
                       args);
    }
    if (args["-m"] == "tabu")
    {
        experiment_run([&](auto args) {
            return tabu(problem, (args.count("-n") ? stoi(args["-n"]) : 10000), (args.count("-tabusize") ? stoi(args["-tabusize"]) : 1000));
            ;
        },
                       args);
    }
    if (args["-m"] == "full")
    {
        auto start = std::chrono::steady_clock::now();
        auto best = brute_force(problem);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "full: " << elapsed_seconds.count() << " " << best << endl;
    }
    if (args["-m"] == "sim_annealing")
    {
        experiment_run([&](auto args) {
            int iterations = (args.count("-n")) ? stoi(args["-n"]) : 10000;
            auto ret_list = sim_annealing(problem, iterations,
                                          [&](int k) {
                                              return 1000.0 * iterations / k;
                                          });
            return ret_list;
        },
                       args);
    }

    return 0;
}
