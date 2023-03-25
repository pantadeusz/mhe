#!/bin/bash

export LANG=C

for i in __summary_*.txt; do
echo "mashing $i"
datamash -t' ' --sort --group 3,4,5,6,7,8 mean 9,10 < $i > $i.averages_times_results.csv
done
