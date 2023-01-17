#!/bin/bash

SUMMARY_STATISTICS=stats_all.txt
AVERAGES=stats_avg.txt
MAXIMUMS=stats_max.txt

#NAMES=( ga_par_opt ga_par ga_seq ga_seq_opt )
NAMES=( ga_par_opt )

POP_SIZE=2000
ITERATIONS=200



for repeat in `seq 0 5`; do
for P_CROSSOVER in 0.1 0.2 0.3 0.4; do
for P_MUTATION in 0.001 0.01 0.1 0.5; do
for version in "${NAMES[@]}"; do
        ./$version -print_stats true -pop_size $POP_SIZE -iterations $ITERATIONS -p_crossover $P_CROSSOVER -p_mutation $P_MUTATION >> $SUMMARY_STATISTICS
done
done
done
done

LANG=C datamash -t' ' --sort --group 1,2,3,4,5 mean 6,7,8,9,10 < $SUMMARY_STATISTICS > $AVERAGES
LANG=C datamash -t' ' --sort --group 1,2,3,4,5 max 6,7,8,9,10 < $SUMMARY_STATISTICS > $MAXIMUMS
