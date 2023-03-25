#include <functional>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

void robimycos(function<void()> f) {
    for (int i = 0; i < 3; i++) {
        f();
        cout << endl;
    }
}

function<void()> fun_factory() {
    shared_ptr<int> x(new int, [](auto p){
        cout << "kasujemy" << endl;
        delete p;
        });
    
    return [=](){
        (*x)++;
        std::cout << (*x) << std::endl;
    };
}
int main(int argc, char **argv) {
    
    auto f1 = [=](){
        std::cout << argc << std::endl;
    };

    int x = 123;
    auto f_x1 = [=](){
        std::cout << x << std::endl;
    };
    auto f_x3 = [&](){
        std::cout << x << std::endl;
    };

    vector<function<void()> > functions = {f_x1, f_x3, fun_factory()};
    for (auto f: functions) robimycos(f);
    cout << "k1" << endl;
    functions = {};
    cout << "k2" << endl;
    
    return 0;
}