#ifndef __SOLVER__SIMULATED_ANNEALING_HPP____
#define __SOLVER__SIMULATED_ANNEALING_HPP____
#include <cmath>
#include <iostream>
#include <map>
#include <list>

// minimalizujemy funkcje f
auto simulated_annealing = [](auto gen_random_sol, // losowy punkt startu 
                              auto get_random_neighbour, // element z sasiedztwa
                              auto T, // funkcja temperatury
                              auto f, // funkcja celu
                              auto u, // zmienna losowa [9,1)
                              const int K = 10000) {
  using namespace std;
  int i = 0;
  auto s0 = gen_random_sol();
  map<int, decltype(s0)> s = {{0, s0}};
  map<int, decltype(s0)> t;
  list<decltype(s0)> V = {s0};

  for (int k = 1; k < K; k++) {
    t[k] = get_random_neighbour(s[k - 1]);
    if (f(t[k]) <= f(s[k - 1])) {
      s[k] = t[k];
      V.push_back(s[k]);
    } else {
      if (u(k) < exp(-abs(f(t[k]) - f(s[k - 1])) / T(k))) {
        s[k] = t[k];
        V.push_back(s[k]);
      } else {
        s[k] = s[k - 1];
      }
    }
  }
  return *max_element(std::begin(V), std::end(V),
                     [&f](auto a, auto b) { return f(a) > f(b); });;
};

#endif
