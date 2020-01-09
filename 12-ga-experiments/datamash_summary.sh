#!/bin/bash

for i in __summary_*.txt; do
echo "mashing $i"
datamash -t' ' --sort --group 2,3,4,5,6,7 mean 8,9 < $i > $i.averages.csv
done
