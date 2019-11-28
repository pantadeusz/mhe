#!/bin/bash

g++ -std=c++17 salesman.cpp -o salesman_experiment1
METHODS_TO_TEST=`./salesman_experiment1 -method list 2> /dev/null`
echo $METHODS_TO_TEST

series_name=`date +%F_%H_%M`
repeat_count=10

for experiment_input_size in `seq 3 11`; do
brute_force_string=""
for i in `seq 1 $repeat_count`; do
    #./salesman_experiment1 input.json output.json brute_force_find_solution
    for method in $METHODS_TO_TEST; do
        input_file=input/${experiment_input_size}.json
        output_file=output/${series_name}/${method}-${experiment_input_size}-$i.json
        output_file_txt=output/${series_name}/${method}-${experiment_input_size}-$i.txt
        mkdir -p `dirname ${output_file}`
        if [ "$method" != "$brute_force_string" ]; then
        #echo "./salesman_experiment1 -in $input_file -out $output_file -method $method 2> $output_file_txt"
        ./salesman_experiment1 -in $input_file -out $output_file -method $method 2> $output_file_txt
        fi
        brute_force_string="brute_force_find_solution"
    done
done
done

# for i in `./salesman_experiment1 -method list 2> /dev/null`; do nodejs statistics.js | grep $i > $i.summary.txt; done
