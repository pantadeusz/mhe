#!/bin/bash


METHODS_PARAMS=( "tabu" "hill_climb" "hill_climb_det" "sim_annealing" )
ITERATIONS=( 100 200 300 400 500 600 700 1000)
PROBLEM_SIZES=( 20 30 40 50 100)
mkdir -p results

for iterations in "${ITERATIONS[@]}"; do 
for problemsize in "${PROBLEM_SIZES[@]}"; do 
for method in "${METHODS_PARAMS[@]}"; do
FILENAME="results/${problemsize}-${method}-${iterations}.txt"
echo "#$method $problemsize ${iterations}" > $FILENAME
for repeat_i in `seq 1 2`; do
echo "$method $iterations $problemsize"
./mhe -f "input/input_${problemsize}.txt" -m $method -n $iterations | sed 's/#[a-z_]*//g' >> $FILENAME
done
python avgs.py $FILENAME | tr '(' ' ' | tr ',' ' ' | tr ')' ' ' > $FILENAME.avg
echo "$iterations $(cat $FILENAME.avg | awk '{print $2}')" >> "results/$problemsize-$method.quality"
done
done
done
