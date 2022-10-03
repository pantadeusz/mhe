
echo "simAnnealing"; 
python ./tsp.py p90 i10000 simAnnealing > simAnnealing.txt
echo "hillClimbingDeterministic"; 
python ./tsp.py p90 i10000 hillClimbingDeterministic > hillClimbingDeterministic.txt
echo "hillClimbingRandomized"; 
python ./tsp.py p90 i10000 hillClimbingRandomized > hillClimbingRandomized.txt
echo "randomProbe"; 
python ./tsp.py p90 i10000 randomProbe > randomProbe.txt
