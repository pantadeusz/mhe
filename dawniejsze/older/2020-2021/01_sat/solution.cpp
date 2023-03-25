#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

/// mając formułę logiczną f(v) znajdujemy takie v ze f(v) == true
// f(v) = v_1 && v_2

bool fun1(vector<bool> v)
{
    return false;
}
bool fun2(vector<bool> v)
{
    return true;
}
bool fun3(vector<bool> v)
{
    return accumulate(v.begin(), v.end(), true, [](auto a, auto b) { return a && b; });
}

bool fun4(vector<bool> v)
{
    static vector<bool> mod;
    if (v.size() != mod.size())
    {
        random_device rd;
        mt19937 rnd(rd());
        mod.resize(v.size());
        for (int i = 0; i < mod.size(); i++)
        {
            mod[i] = (rnd() % 2);
        }
    }
    bool ret = true;
    for (int i = 0; i < v.size(); i++)
    {
        bool x;
        if (mod.at(i))
            x = v[i];
        else
            x = ~v[i];
        ret = ret && x;
    }
    return ret;
}

auto sat_solver = [](auto f, int n) -> vector<bool> {
    vector<bool> v(n);
    do
    {
        if (f(v))
            return v;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i])
            {
                v[i] = false;
            }
            else
            {
                v[i] = true;
                break;
            }
        }
        // sprawdzamy czy wszędzie jest wartość true, jeśli tak to kończymy
    } while (!accumulate(v.begin(), v.end(), true, [](auto a, auto b) { return a && b; }));
    if (f(v))
        return v;
    // f f t f -> ((f & f) & t) & f -> f
    // t t t t -> ((t & t) & t) & t -> t
    return {};
};

int main(int argc, char **argv)
{
    auto result = sat_solver(fun4, stod(argv));
    cout << "result size " << result.size() << endl;
    for (auto e : result)
    {
        cout << (e ? "T" : "F");
    }
    cout << endl;

    return 0;
}
