#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
#include <chrono>

using namespace std;
ostream &operator<<(ostream &o, const vector<bool> &v) {
  for (auto e : v)
    o << e ? "t" : "f";
  return o;
}

/**
 * returns {} if not satisfiable, or vector with example that gievs true
 * */
auto sat_solver = [](auto f, int varialbe_count) -> vector<bool> {
  vector<bool> x(varialbe_count);
  do {
    if (f(x))
      return x;
    for (int i = 0; i < x.size(); i++) {
      if (x[i]) {
        x[i] = false;
      } else {
        x[i] = true;
        break;
      }
    }
  } while (!accumulate(x.begin(), x.end(), true,
                       [](bool a, bool b) { return a && b; }));
  if (f(x))
    return x;
  return {};
};

int main(int argc, char **argv) {
  if (argc > 1) {
    auto start = std::chrono::system_clock::now();
    cout << "result: "
         << sat_solver([](vector<bool> v) { return v.back(); }, stoi(argv[1]))
         << endl;
    auto end = std::chrono::system_clock::now();
    cerr << stoi(argv[1]) << " " << std::chrono::duration<double>(end - start).count() << endl;
  } else {
    cout << "how big is the problem?" << endl;
  }
  return 0;
}
