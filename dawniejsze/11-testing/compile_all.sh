

g++ -std=c++17 ga.cpp -fopenmp -O3 -o ga_par_opt
g++ -std=c++17 ga.cpp -fopenmp -o ga_par

g++ -std=c++17 ga.cpp -O3 -o ga_seq_opt
g++ -std=c++17 ga.cpp -o ga_seq