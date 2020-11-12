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
repeat_count=25

for i in `seq 1 $repeat_count`; do
for experiment_input_size in `seq 3 11`; do
echo "repeat numbe $i"
for mutation_p in 0.0 0.01 0.05 0.1 0.3; do
for crossover_p in 1.0 0.8 0.7 0.2 0.0; do
for selection in tournament_selection roulette_selection rank_selection; do
for crossover in crossover_two_point crossover_one_point; do
for pop_size in 10 20 40 60 100 200 300 500; do # 750 1000; do
    method="genetic_algorithm"
    experiment_id=${method}=$i=${experiment_input_size}=${selection}=${crossover}=${pop_size}=${mutation_p}=${crossover_p}
        input_file=input/${experiment_input_size}.json
        output_file=output/${series_name}/${experiment_id}.json
        output_file_txt=output/${series_name}/${experiment_id}.txt
        mkdir -p `dirname ${output_file}`
        echo ${experiment_id}
        $EXECUTABLE_NAME -in $input_file -out $output_file -method $method \
           -population_size $pop_size \
           -mutation_probability $mutation_p \
           -crossover_probability $crossover_p \
            -crossover $crossover \
            -selection $selection \
            2> $output_file_txt
done # crossover
done # selection
done # crossover_p
done # mutation_p
done # pop_size
done
done

