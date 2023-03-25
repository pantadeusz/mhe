#include <functional>
#include <iostream>
#include <vector>

auto sat_solver = [](auto formula, int n) {
    using namespace std;

    vector<bool> valuation(n);
    do {
        if (formula(valuation)) return valuation;
        int i = 0;
        do {
            if (valuation[i])
                valuation[i] = false;
            else {
                valuation[i] = true;
                break;
            }
            i++;
        } while (i < n);
        if (i >= n) break;
    } while (true);

    return vector<bool>{};
};

std::ostream &operator<< (std::ostream &o, std::vector<bool> v)
{
    for (auto e : v) {
        o << ((e) ? "true" : "false") << " ";
    }
    return o;
}

int main(int argc, char **argv)
{
    using namespace std;

    auto f1 = [](vector<bool> v) {
            return v[0] || v[1];
        }; 
    auto f2 = [](vector<bool> v){
        bool result = v[0];
        for (int i = 0; i < v.size(); i++) {
            result = result && v[i];
        }
        return result;
    };
    auto f3 = [](vector<bool> v){
        bool result = v[0];
        for (int i = 0; i < v.size(); i++) {
            result = result || v[i];
        }
        return result;
    };
    auto result = sat_solver(
        f2,
        stoi(argv[1]));
    cout << result << endl;
    return 0;
}
