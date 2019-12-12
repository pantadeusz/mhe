#!/bin/bash

EXECUTABLE_NAME=./build_experiment/salesman
# ARGC="-std=c++17"
# g++ $ARGC salesman.cpp -o $EXECUTABLE_NAME
mkdir -p build_experiment
cd build_experiment
cmake ..
make salesman
cd ..
METHODS_TO_TEST=`$EXECUTABLE_NAME -method list 2> /dev/null`
echo $METHODS_TO_TEST

series_name=`date +%F_%H_%M`




node statistics.js > /tmp/stats__190865.txt
echo "set term png" > output_chart.plt

## results
echo "set output \"output_chart_results.png\"" >> output_chart.plt
echo "" >> output_chart.plt
echo -n "plot " >> output_chart.plt 
for method in $METHODS_TO_TEST; do
cat /tmp/stats__190865.txt | grep $method > output_stats__$method.txt 
echo -n "'output_stats__${method}.txt' using 2:4 w lines title \"$method\" , " >> output_chart.plt 
done
echo "" >> output_chart.plt


echo "set output \"output_chart_times.png\"" >> output_chart.plt
echo "set logscale y 2" >> output_chart.plt
echo -n "plot " >> output_chart.plt 
for method in $METHODS_TO_TEST; do
cat /tmp/stats__190865.txt | grep "$method " > output_stats__$method.txt 
echo -n "'output_stats__${method}.txt' using 2:6 w lines title \"$method\" , " >> output_chart.plt 
done
echo "" >> output_chart.plt

gnuplot output_chart.plt

