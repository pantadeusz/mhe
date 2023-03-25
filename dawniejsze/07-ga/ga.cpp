#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <chrono>

std::random_device rd;
std::mt19937_64 rd_generator(rd());

struct city_t {
    // {latitude, longitude}
    std::string name;
    std::vector<double> coordinates;
};

using problem_t = std::vector<city_t>;

class solution_t : public std::vector<int>
{
public:
    std::shared_ptr<problem_t> problem_p;
};


std::vector<double> operator*(const std::vector<double> a, const double b)
{
    std::vector<double> result = a;
    for (auto& x : result)
        x = x * b;
    return result;
}
std::vector<double> operator+(const std::vector<double> a, const std::vector<double> b)
{
    assert(a.size() == b.size());
    std::vector<double> result = a;
    for (int i = 0; i < b.size(); i++)
        result[i] += b.at(i);
    return result;
}
std::vector<double> operator-(const std::vector<double> a, const std::vector<double> b)
{
    return a + (b * -1.0);
}

double count_cost(const problem_t& problem, const solution_t& solution)
{
    double length = 0.0;
    for (int i = 0; i < solution.size(); i++) {
        auto v = problem[solution[i]].coordinates - problem[solution[(i + 1) % solution.size()]].coordinates;
        double l = 0;
        for (auto& x : v)
            l += x * x;
        length += std::sqrt(l);
    }
    return length;
}

double fitness(const solution_t solution)
{
    return 1.0 / (1.0 + count_cost(*solution.problem_p, solution));
}

solution_t mutation(const solution_t& solution)
{
    std::uniform_int_distribution<int> distr(0, solution.size() - 1);
    int a(distr(rd_generator)), b(distr(rd_generator));
    auto ret = solution;
    std::swap(ret[a], ret[b]);
    return ret;
}

std::string utc(const std::time_t time)
{
    const auto l = std::string("yyyy-mm-ddThh:mm:ssZ").size();
    char timeString[l];
    std::strftime(timeString, l, "%FT%TZ", std::gmtime(&time));
    return timeString;
}

std::ostream& operator<<(std::ostream& o, const solution_t& solution)
{
    std::time_t time = std::time({});
    o << R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<gpx xmlns="http://www.topografix.com/GPX/1/1" xmlns:gpxx="http://www.garmin.com/xmlschemas/GpxExtensions/v3" xmlns:gpxtpx="http://www.garmin.com/xmlschemas/TrackPointExtension/v1" creator="Oregon 400t" version="1.1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd">
  <metadata>
    <link href="http://www.garmin.com">
      <text>Garmin International</text>
    </link>
    <time>)"
      << utc(time) << R"(</time>
  </metadata>
  <trk>
    <name>TSP Track</name>
    <trkseg>)";
    for (auto sol_element : solution) {
        o << "<trkpt lat=\"" << solution.problem_p->at(sol_element).coordinates[0] << "\" lon=\"" << solution.problem_p->at(sol_element).coordinates[1] << "\">\n";
        o << "<ele>200</ele>\n";
        o << "<time>" << utc(time) << "</time>\n";
        o << "</trkpt>\n";
    }
    o << R"(</trkseg>
  </trk>
</gpx>
    )";
    return o;
}
std::istream& operator>>(std::istream& o, city_t& city)
{
    city.coordinates.resize(2);
    o >> city.name >> city.coordinates[0] >> city.coordinates[1];
    return o;
}
std::istream& operator>>(std::istream& o, problem_t& problem)
{
    int count;
    o >> count;
    for (int i = 0; i < count; i++) {
        city_t city;
        o >> city;
        problem.push_back(city);
    }
    return o;
}

solution_t dummy_solution_for_problem(problem_t p)
{
    solution_t solution;
    solution.problem_p = std::make_shared<problem_t>();
    *(solution.problem_p) = p;
    solution.resize(p.size());
    std::iota (std::begin(solution), std::end(solution), 0);
    return solution;
}
int main(int argc, char** argv)
{
    problem_t problem;
    std::cin >> problem;
    auto solution = dummy_solution_for_problem(problem);
    std::cout << solution;
    return 0;
}