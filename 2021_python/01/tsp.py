import math
import itertools
import random
# def. problemu
# def. rozwiazania - dziedzina funkcji
# f. celu 




def goalFunction (solution, problem):
    '''naturalna reprezentacja rozwiazania - lista kolejnych miast do odwiedzenia'''
    sum = 0
    for i in range(0,len(solution)):
        p0 = problem[solution[i]]
        p1 = problem[solution[(i+1)%len(solution)]]
        sum = sum + math.sqrt((p0[0]-p1[0])**2 + (p0[1] - p1[1])**2)
    return sum

def generateRandomVisitOrder(n):
    r = []
    f = list(range(0,n))
    for i in range(0,n):
        p = int (random.uniform(0,len(f)-1))
        r.append(f[p])
        del f[p]
        #print(f)
        #print(r)
    return r

def fullSearch(goal, problem):
    f = list(range(0,len(problem)))
    currentBest = f
    for newSol in itertools.permutations(f):
        #print(newSol)
        if (goal(newSol) < goal(currentBest)):
            currentBest = newSol
    return currentBest

def randomProbe(goal, gensol, iterations):
    currentBest = gensol()
    for i in range(0,iterations):
        newSol = gensol()
        if (goal(newSol) < goal(currentBest)):
            currentBest = newSol
        print (goal(currentBest))
    return currentBest

def generateProblem(n):
    cities = []
    for i in range(0,n):
        cities.append([random.uniform(0,100),random.uniform(0,100)])
    return cities

# problem = [[0,0],[1,0],[0.5,1.1],[1,1],[0,1]]
problem = generateProblem(5)
print (problem)
#print (generateRandomVisitOrder(4))
#print (goalFunction([0,2,1,3],problem))

sol = randomProbe(lambda s : goalFunction(s, problem), lambda : generateRandomVisitOrder(len(problem)), 3)
print (sol)
print (goalFunction(sol, problem))
if (len(problem) < 10):
    print (fullSearch(lambda s : goalFunction(s, problem),problem))
