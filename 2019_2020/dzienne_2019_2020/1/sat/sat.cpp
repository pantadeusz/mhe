#include <iostream>
#include <vector>

using namespace std;

ostream &operator<<(ostream &o, const vector<bool> &x) {
  for (auto e : x)
    o << (e ? "true" : "false") << " ";
  return o;
}

// rozmiar problemu do rozwiązania (liczba zmiennych zdaniowych dla formuły
// logicznej)
//    unsigned n = 10;
auto solve_sat = [](auto fun, int n) -> vector<bool> {
  // generujemy kolejne wartościowania
   vector<bool> ret = {};
  #pragma omp parallel for
  for (unsigned long long i = 0; i < (1 << n); i++) {
    vector<bool> x(n); // zmienne zdaniowe
    for (int p = 0; p < n; p++) {
      x[p] = (i & (1 << p)) != 0; // przypisujemy odpowiednio prawdę lub fałsz w
                                  // zależności od wartości bitu na
      // zadanej pozycji p
    }
    if (fun(x)) {
      // cout << "dla " << x << " jest " << fun(x) << endl;
      #pragma omp critical
      ret = x;
    }
  }
  return ret;
};

int main(int argc, char **argv) {
  // auto f1 = [](vector<bool> x) {
  //    return (x[0] || x[1]) && x[2] && !x[2];
  //};

  auto f2 = [](vector<bool> x) {
    bool ret = true;
    for (auto e : x) {
      ret = ret && e;
    }
    return ret;
  };
  if (argc < 2) cout << "podaj rozmiar problemu!" << endl;
  vector<bool> x(stoi(argv[1])); // zmienne zdaniowe
  cout << "wynik sat( " << stoi(argv[1]) << " ): " << (x = solve_sat(f2, stoi(argv[1]))) << endl;
  cout << "wyrażenie jest spełnialne ( " << stoi(argv[1]) << " ): " << x << " daje " << f2(x) << endl;
  return 0;
}