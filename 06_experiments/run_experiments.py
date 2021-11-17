import os;
import re;
import numpy as np;

statistics = {
    "tabu_search":[],
    "hill_climb_rnd":[]
}

for method_name in "tabu_search","hill_climb_rnd":
    for problem_size in range(5, 30):
        for repeat in range(1,15):
            cmndName = "./a.out --size " + str(problem_size) +  " --method " + method_name + " --iterations 10000 --tabu_size 20"
            #print(cmndName)
            result = os.popen(cmndName)
            output = result.read()
            # print(output)
    #        statistics[method_name][problem_size] = output
            calcTime = re.findall("dt.*", output)
            #print(calcTime)
            if (len(calcTime) > 0):
                calcTime = re.findall("[0123456789.]+",calcTime[0])
                #print(float(calcTime[0]))

                result_val = re.findall("result.*", output)
                result_val = re.findall("[0123456789.]+",result_val[0])

                statistics[method_name].append([problem_size,float(result_val[0]), float(calcTime[0])])


#print(statistics)
for method_name in statistics:
    print(method_name)
    summary = statistics[method_name]
    #print(summary)
    per_size = {}
    for values in summary:
        if (per_size.get(values[0]) is None):
            per_size[values[0]] = [[values[1], values[2]]] 
        else:
            per_size[values[0]].append([values[1], values[2]])
    #print(per_size)
    for s in per_size:
        combined = np.mean(per_size[s], axis=0)
        print(str(s) + " "  + str(combined[0]) + " "+ str(combined[1]))
