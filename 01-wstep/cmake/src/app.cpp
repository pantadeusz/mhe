#include <iostream>
#include <string>
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

int main(int argc, char **argv) {
  using namespace std;
  MojaKlasa x = {99};
  x.ktoja();

  auto y = wypisz;

  y();

  return 0;
}
