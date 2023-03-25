import math
import itertools
import random
import copy
import sys
import re
# def. problemu
# def. rozwiazania - dziedzina funkcji
# f. celu


def goalFunction(solution, problem):
    '''naturalna reprezentacja rozwiazania - lista kolejnych miast do odwiedzenia'''
    sum = 0
    for i in range(0, len(solution)):
        p0 = problem[solution[i]]
        p1 = problem[solution[(i+1) % len(solution)]]
        sum = sum + math.sqrt((p0[0]-p1[0])**2 + (p0[1] - p1[1])**2)
    return sum

def printSolution(i, currentBest, goal):
    print("" + str(i) + " " + str(goal(currentBest)))

def doNotPrintSolution(i, currentBest, goal):
    return 0

def generateRandomVisitOrder(n):
    r = []
    f = list(range(0, n))
    for i in range(0, n):
        p = int(random.uniform(0, len(f)-1))
        r.append(f[p])
        del f[p]
        # print(f)
        # print(r)
    return r


def fullSearch(goal, problem, onIteration):
    f = list(range(0, len(problem)))
    currentBest = f
    i = 0
    for newSol in itertools.permutations(f):
        # print(newSol)
        if (goal(newSol) < goal(currentBest)):
            currentBest = newSol
        onIteration(i, currentBest, goal)
        i = i + 1
    return currentBest


def randomProbe(goal, gensol, iterations, onIteration):
    currentBest = gensol()
    for i in range(0, iterations):
        newSol = gensol()
        if (goal(newSol) < goal(currentBest)):
            currentBest = newSol
        onIteration(i, currentBest, goal)
    return currentBest


def hillClimbingRandomized(goal, gensol, genNeighbour, iterations,onIteration):
    '''goal - funkcja celu (to optymalizujemy),
    gensol - generowanie losowego rozwiazania,
    genNeighbour - generowanie losowego punktu z otoczenia rozwiazania,
    iterations - liczba iteracji alg.'''
    currentBest = gensol()
    for i in range(0, iterations):
        newSol = genNeighbour(currentBest)
        if (goal(newSol) <= goal(currentBest)):
            currentBest = newSol
        onIteration(i, currentBest, goal)
    return currentBest

def hillClimbingDeterministic(goal, gensol, genBestNeighbour, iterations,onIteration):
    '''goal - funkcja celu (to optymalizujemy),
    gensol - generowanie losowego rozwiazania,
    genNeighbour - generowanie losowego punktu z otoczenia rozwiazania,
    iterations - liczba iteracji alg.'''
    currentBest = gensol()
    for i in range(0, iterations):
        newSol = genBestNeighbour(currentBest, goal)
        if (newSol == currentBest):
            return currentBest
        currentBest = newSol
        onIteration(i, currentBest, goal)
    return currentBest


def simAnnealing(goal, gensol, genNeighbour, T, iterations, onIteration):
    '''goal - funkcja celu (to optymalizujemy),
    gensol - generowanie losowego rozwiazania,
    genNeighbour - generowanie losowego punktu z otoczenia rozwiazania,
    iterations - liczba iteracji alg.'''
    currentBest = gensol()
    V = [currentBest]
    for i in range(1, iterations+1):
        newSol = genNeighbour(currentBest)
        if (goal(newSol) <= goal(currentBest)):
            currentBest = newSol
            V.append(currentBest)
        else:
            e = math.exp(- abs(goal(newSol) - goal(currentBest))/T(i))
            u = random.uniform(0.0,1.0)
            if (u < e):
                currentBest = newSol
                V.append(currentBest)
        onIteration(i-1, currentBest, goal)
    
    return min(V, key=goal)


def generateProblem(n):
    '''tworzy przykladowe zadanie dla n miast '''
    cities = []
    for i in range(0, n):
        cities.append([random.uniform(0, 100), random.uniform(0, 100)])
    return cities


def getRandomNeighbour(currPoint):
    swapPoint = int(random.uniform(0, len(currPoint)-1))
    newPoint = copy.deepcopy(currPoint)
    newPoint[(swapPoint+1) % len(currPoint)] = currPoint[swapPoint]
    newPoint[swapPoint] = currPoint[(swapPoint+1) % len(currPoint)]
    return newPoint

def getRandomNeighbour2(currPoint):
    for i in range(0, int(min(abs( random.normalvariate(0.0,2.0)) + 1,500)) ):
        swapPoint = int(random.uniform(0, len(currPoint)-1))
        newPoint = copy.deepcopy(currPoint)
        newPoint[(swapPoint+1) % len(currPoint)] = currPoint[swapPoint]
        newPoint[swapPoint] = currPoint[(swapPoint+1) % len(currPoint)]
        currPoint = newPoint
    return currPoint

def getBestNeighbour(currPoint, goal):
    best = currPoint
    for swapPoint in range(0,len(currPoint)-1):
        newPoint = copy.deepcopy(currPoint)
        newPoint[(swapPoint+1) % len(currPoint)] = currPoint[swapPoint]
        newPoint[swapPoint] = currPoint[(swapPoint+1) % len(currPoint)]
        if (goal(newPoint) <= goal(best)):
            best = newPoint
    return best



########################################3###############################
# problem = [[0,0],[1,0],[0.5,1.1],[1,1],[0,1]]
problem = generateProblem(5)
#print (generateRandomVisitOrder(4))
#print (goalFunction([0,2,1,3],problem))

iterations = 500
printer = printSolution
for arg in sys.argv:
    if re.search("p[0-9][0-9]*", arg) != None:
        problem = generateProblem(int(arg[1:]))
    if re.search("i[0-9][0-9]*", arg) != None:
        iterations = int(arg[1:])
    if arg == '-problem':
        print(problem)
    if arg == '-print':
        printer=printSolution
    if arg == '-noprint':
        printer=doNotPrintSolution
    if arg == 'randomProbe':
        # Random probe
        sol = randomProbe(lambda s: goalFunction(s, problem),
                        lambda: generateRandomVisitOrder(len(problem)), iterations, printer)
    if arg == 'hillClimbingRandomized':
        # wspinaczka
        sol = hillClimbingRandomized(lambda s: goalFunction(s, problem),
                                     lambda: generateRandomVisitOrder(len(problem)),
                                     getRandomNeighbour, iterations, printer)
    
    if arg == 'hillClimbingDeterministic':
        # wspinaczka
        sol = hillClimbingDeterministic(lambda s: goalFunction(s, problem),
                                     lambda: generateRandomVisitOrder(len(problem)),
                                     getBestNeighbour, iterations, printer)
    if arg == 'simAnnealing':
        # wspinaczka
        sol = simAnnealing(lambda s: goalFunction(s, problem),
                                     lambda: generateRandomVisitOrder(len(problem)),
                                     getRandomNeighbour2, 
                                     lambda k : 1000.0/k,
                                     iterations, printer)
    if arg == 'fullSearch':
        sol = fullSearch(lambda s: goalFunction(s, problem), problem, printer)
    if arg == '-printSol':
        print(sol)
        print(goalFunction(sol, problem))

