#ifndef __PROBLEM_DEFINITION_HPP___
#define __PROBLEM_DEFINITION_HPP___
#include <memory>

/**
 * ogólny typ rozwiązania
 * */
class solution_t{};
/**
 * rozwiązanie opakowane jako smartpointer
 * */
using solution_p = std::shared_ptr<solution_t>;

/**
 * generowanie następnego poprawnego rozwiązania. Powinno to stworzyć nowy obiekt będący klonem tego porzedniego
 * */
using next_valid_solution_f =  solution_p (*) (solution_p);

/**
 * Funkcja celu przyjmuje referencje do rozwiązania
 * */
using goal_minimize_f = double (*) (solution_p); 


#endif
