# do_chart print_input print_result print_result_eval    
#!/bin/bash

method=brute_force
echo -n "$method "
echo -n "$(./a.out $method 500 false false false true) "
echo

for method in random_probe hill_climb_det tabu_search; do
    echo -n "$method "
    for i in `seq 1 25`; do
        echo -n "$(./a.out $method 500 false false false true) "
    done
    echo
done
