#!/bin/bash

EXECUTABLE_NAME=./build_experiment/salesman_opt
# ARGC="-std=c++17"
# g++ $ARGC salesman.cpp -o $EXECUTABLE_NAME
mkdir -p build_experiment
cd build_experiment
cmake ..
make $(basename $EXECUTABLE_NAME)
cd ..
METHODS_TO_TEST=`$EXECUTABLE_NAME -method list 2> /dev/null`
echo $METHODS_TO_TEST

series_name=`date +%F_%H_%M`
repeat_count=10

for experiment_input_size in `seq 3 11`; do
brute_force_string=""
for i in `seq 1 $repeat_count`; do
    #$EXECUTABLE_NAME input.json output.json brute_force_find_solution
    for method in $METHODS_TO_TEST; do
        input_file=input/${experiment_input_size}.json
        output_file=output/${series_name}/${method}-${experiment_input_size}-$i.json
        output_file_txt=output/${series_name}/${method}-${experiment_input_size}-$i.txt
        mkdir -p `dirname ${output_file}`
        if [ "$method" != "$brute_force_string" ]; then
        echo "$EXECUTABLE_NAME -in $input_file -out $output_file -method $method -population_size 220 -iteration_count 50 -mutation_probability 0.1"
        #echo "$EXECUTABLE_NAME -in $input_file -out $output_file -method $method 2> $output_file_txt"

        $EXECUTABLE_NAME -in $input_file -out $output_file -method $method \
           -population_size 220 -iteration_count 50 \
           -mutation_probability 0.1\
           -crossover_probability 0.7 \
            -crossover crossover_two_point \
            -selection rank_selection \
            2> $output_file_txt
        fi
        brute_force_string="brute_force_find_solution"
    done
done
done

# for i in `$EXECUTABLE_NAME -method list 2> /dev/null`; do nodejs statistics.js | grep $i > $i.summary.txt; done

