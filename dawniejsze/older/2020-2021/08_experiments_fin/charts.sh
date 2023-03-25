#!/bin/bash

cd results
FILENAMES=(100-*.quality)
echo "${FILENAMES[@]}"
cat ../charts.template | sed "s/FILENAMES/${FILENAMES[*]}/g" > 100-chart.plt
gnuplot 100-chart.plt
