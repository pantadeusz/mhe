#ifndef ___HELPER__SALESMAN__HPP____
#define ___HELPER__SALESMAN__HPP____

#include <map>
#include <string>
#include <vector>
#include <iostream>

auto process_arguments = [](int argc, char **argv) -> std::map<std::string, std::string>{
  using namespace std;
  map<string, string> arguments_map;
  for (auto a : vector<string>(argv + 1, argv + argc)) {
    static string k = "";
    if (a.substr(0, 1) == "-") {
      k = string(a.begin() + 1, a.end());
      arguments_map[k] = "";
    } else
      arguments_map[k] = a;
  }
  // for (auto [k,v]:arguments_map) {
  //   std::cout << k << " " << v << std::endl;
  // }
  if (arguments_map.count("h") + arguments_map.count("H") +
      arguments_map.count("help")) {
    cout << "arguments:" << endl;
    cout << "  -in [input]" << endl;
    cout << "  -out [output]" << endl;
    cout << "  -method [method_name]" << endl;
    cout << "  -html [html_visualization_file_name]" << endl;
    throw invalid_argument("help was needed");
  }
  return arguments_map;
};

#endif
