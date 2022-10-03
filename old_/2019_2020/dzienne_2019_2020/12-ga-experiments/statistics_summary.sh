#!/bin/bash

P=$PWD
echo "collecing results"
for current_dir in output/*; do
cd $current_dir
for i in *.txt; do echo ${i%.*}=$(cat $i | grep calculation_time | awk '{print $3}')=$(cat $i | grep goal_value | awk '{print $3}') | tr '=' ' ' ; done > $P/__summary_$(basename $current_dir).txt
cd $P
done