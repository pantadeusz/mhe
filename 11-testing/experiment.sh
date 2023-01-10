#!/bin/bash


POP_SIZE=2000
ITERATIONS=200
P_CROSSOVER=0.01
P_MUTATION=0.0001

NAMES=( ga_par_opt ga_par ga_seq ga_seq_opt )

for version in "${NAMES[@]}"; do
    ./$version -print_stats true -pop_size $POP_SIZE -iterations $ITERATIONS -p_crossover $P_CROSSOVER -p_mutation $P_MUTATION
done
