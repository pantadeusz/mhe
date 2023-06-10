#!/bin/bash -e

P_MUTATION="0 0.2 0.5 0.9"
P_CROSSOVER="0 0.2 0.5 1.0"
REPEATS=10

rm -f results.csv
for i in `seq 1 $REPEATS`; do
for p_cross in $P_CROSSOVER; do
for p_mut in $P_MUTATION; do
GOAL=`./build/mhe -count_time -conv_curve 0 -iterations 100 -pop_size 2000 -p_crossover $p_cross -p_mutation $p_mut -result_fit`
echo "$p_cross $p_mut $GOAL" >> results.csv
done
done
done
LANG=C datamash -t' ' --sort --group 1,2 mean 3,4 < results.csv > means.csv
