#include <functional>
#include <iostream>
#include <vector>

std::ostream &operator<<(std::ostream &o, const std::vector<double> &v) {
  for (auto e : v)
    o << " " << e;
  return o;
}
std::vector<double> operator+(const std::vector<double> x,
                              const std::vector<double> y) {
  std::vector<double> wynik(x.size());
  for (int i = 0; i < std::min(x.size(), y.size()); i++) {
    wynik[i] = x[i] + y[i];
  }
  return wynik;
}

std::function<void()> new_function(double x = 0, double y = 0) {
  using namespace std;
  vector<double> a = {x, y};
  return [=]() {
    vector<double> b = {2, 4};
    vector<double> c = a + b;
    cout << a << " + " << b << " = " << c << endl;
  };
}

void f2() {};
void f3() {};

int main() {
  using namespace std;
  function<void()> aaa = f2;
  function<void()> f = new_function(100,100);
  f();
  f = [](){cout << "nic nie robie" << endl;};
  f();
  return 0;
}

#ifdef WYWALAMY
using namespace std;
ostream &operator<<(ostream &o, const vector<bool> &v) {
  for (auto e : v)
    o << e ? "t" : "f";
  return o;
}

bool formula(vector<bool> v) { return v[0] && v[1] && !v[2] && !v[0]; };

auto sat_solver = [](auto formula) {

};

int main() {
  vector<bool> v = {true, true, false};
  cout << v << " => " << formula(v) << endl;
  sat_solver([](vector<bool> a) {});
  return 0;
}

#endif