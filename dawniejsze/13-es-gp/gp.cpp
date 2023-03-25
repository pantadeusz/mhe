#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<string, double> terminals = {{"0", 0.0}, {"1", 1.0}, {"x", 2}, {"y", 3}};
map<string, function<double(vector<double>)>> functions = {
    {"+", [](auto a) { return a.at(0) + a.at(1); }},
    {"-", [](auto a) { return a.at(0) - a.at(1); }},
    {">", [](auto a) { return (a.at(0) > a.at(1)) ? 1.0 : 0.0; }},
    {"==", [](auto a) { return (a.at(0) == a.at(1)) ? 1.0 : 0.0; }},
    {"if-then-else", [](auto a) { return (a.at(0) > 0.5) ? a.at(1) : a.at(2); }}};

vector<string> get_subprogram(vector<string> program, int i0)
{
    //  (  (  (  ) ( ) ) )
    // +1 +1 +1 -1 +1 -1 -1 -1 = 0
    int brackets = 1;
    int i = i0;
    i++;
    while ((brackets > 0) && (i < program.size())) {
        if (program[i] == "(")
            brackets++;
        else if (program[i] == ")")
            brackets--;
        i++;
    }
    return {program.begin() + i0 + 1, program.begin() + i - 1};
}
double evaluate(vector<string> program)
{
    vector<double> values;
    string symbol;
    for (int i = 0; i < program.size(); i++) {
        if (terminals.count(program[i])) {
            values.push_back(terminals[program[i]]);
        } else if (program[i] == "(") {
            auto subprogram = get_subprogram(program, i);
            i += subprogram.size() + 1;
            values.push_back(evaluate(subprogram));
        } else {
            symbol = program[i];
        }
    }
    if (functions.count(symbol)) {
        return functions[symbol](values);
    } else if (values.size() == 1)
        return values[0];
    else
        throw invalid_argument("error");
}

// vector<string> tree_elements;
// map<string*, string*> parents;
// map<string*, vector<string*> > children;

int main(int argc, char** argv)
{
    // + 1 2
    vector<string> program;
    string symbol;

    terminals["x"] = stof(argv[1]);
    terminals["y"] = stof(argv[2]);
    while (cin >> symbol) {
        program.push_back(symbol);
    }
    cout << "result: " << evaluate(program) << endl;
    return 0;
}