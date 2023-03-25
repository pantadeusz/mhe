#include <fstream>
#include <functional>
#include <iostream>
#include <json.hpp>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

class MojaKlasa {
  static int licznik;

 public:
  int id;

  MojaKlasa();
  MojaKlasa(int myID);
  MojaKlasa(MojaKlasa &&m);
  ~MojaKlasa();
  void ktoja();
};

int MojaKlasa::licznik = 0;

MojaKlasa::MojaKlasa() {
  id = licznik++;
  std::cout << "(" << id << ")jestem nowy" << std::endl;
}
MojaKlasa::MojaKlasa(int myID) {
  id = myID;
  std::cout << "(" << id << ")jestem nowy wg ID" << std::endl;
}
MojaKlasa::MojaKlasa(MojaKlasa &&m) {
  id = m.id;
  std::cout << "(" << id << ")jestem kopią" << std::endl;
}

MojaKlasa::~MojaKlasa() {
  std::cout << "(" << id << ")jestem skasowany" << std::endl;
}

void MojaKlasa::ktoja() { std::cout << "Jam ci " << id << std::endl; }

void wypisz() { std::cout << "Jestem sobie funkcja" << std::endl; }

class InnyTyp {
 public:
  int x;
  std::string b;
};

std::function<void()> fabryka() {
  auto i = std::make_shared<int>(10);
  return [=]() {
    std::cout << "i = " << *i << std::endl;
    (*i)++;
  };
}

int main(int argc, char **argv) {
  using namespace std;
  MojaKlasa x = {99};
  x.ktoja();
  auto f = fabryka();
  f();
  vector<InnyTyp> tablica = {{1, "jeden"}, {2, "dwa"}};
  for (auto &[i, c] : tablica) {
    cout << "element: " << i << " " << c << endl;
    i = -i;
  }
  for (auto &[i, c] : tablica) {
    cout << "po zmianie: " << i << " " << c << endl;
  }

  f();
  f();
  f();
  std::ifstream file("test.json");
  nlohmann::json data = nlohmann::json::parse(file);
  std::cout << data.dump() << std::endl;
  nlohmann::json j = {{"sdasf",22},{"r","1122"}};
  std::cout << data["liczba"] << std::endl;
  return 0;
}
