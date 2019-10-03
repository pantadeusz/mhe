#include <string>
#include <vector>
#include <cmath>
#include <iostream>

class city_t {
public:
  std::string name;
  double longitude;
  double latitude;
  double distance(city_t &c2) {
      using namespace std;
    auto &[name2,lon2,lat2] = c2;
    auto lat1 = latitude;
    auto lon1 = longitude;
    auto R = 6371e3; // metres
    auto fi1 = lat1*M_PI/180.0;
    auto fi2 = lat2*M_PI/180.0;
    auto deltafi = (lat2 - lat1)*M_PI/180.0;
    auto deltalambda = (lon2 - lon1)*M_PI/180.0;
    auto a = sin(deltafi / 2) * sin(deltafi / 2) +
             cos(fi1) * cos(fi2) * 
             sin(deltalambda / 2) * sin(deltalambda / 2);
    auto c = 2 * atan2(sqrt(a), sqrt(1 - a));
    auto d = R * c;
    return d;
  }
};

class problem_t {
public:
  std::vector<city_t> cities;
};

class solution_t : public problem_t {
public:
  std::vector<int> cities_to_see; // indeksy miast
  double goal() {
      double sum = 0.0;
      auto prev_city = cities_to_see[0];
      for (auto city: cities_to_see) {
          sum += cities[city].distance(cities[prev_city]);
          prev_city = city;
      }
      return sum;
  };
};

int main() {
    using namespace std;
    
    solution_t experiment;

    experiment.cities = {
        {"Warszawa",52.2330269,20.7810081},
        {"Gdansk",54.3612063,18.5499431}
    };
    experiment.cities_to_see = {0,1};
    cout << experiment.goal()/1000.0 << endl;
}
