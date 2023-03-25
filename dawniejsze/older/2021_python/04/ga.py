from hashlib import new
import random
import copy
import operator
import multiprocessing

def oneMax(chromosome):
    result = 0.0
    for c in chromosome:
        result = result + c
    return result


def crossoverOnePoint(chromosomes):
    [chromosome1, chromosome2] = chromosomes
    r = random.randint(0,len(chromosome1))
    offspring1 = copy.copy(chromosome1)
    offspring2 = copy.copy(chromosome2)
    for i in range(r,len(chromosome1)):
        offspring1[i] = chromosome2[i]
        offspring2[i] = chromosome1[i]
    return [offspring1, offspring2]

def mutation(choromosome, pmutation):
    mutated = copy.copy(choromosome)
    for i in range (len(mutated)):
        if random.uniform(0,1) < pmutation:
            mutated[i] = 1-mutated[1]
    return mutated

def selection(population, fitensses, tournament_size):
    r = random.randint(0,len(population)-1)
    best = [population[r], fitensses[r]]
    # print(best)
    for i in range(1,tournament_size):
        r = random.randint(0,len(population)-1)
        if best[1] < fitensses[r]:
            best = [population[r], fitensses[r]]
       # print(best, fitensses[r])
    return best

def initialPopulation(n, l):
    pop = []
    for i in range(n):
        c = []
        for i in range(l):
            c.append(random.randint(0,1))
        pop.append(c)
    return pop



def geneticAlgorithm(fitnessF, pop_size, chromosome_l, iterations, p_crossover, p_mutation):
    init_pop = initialPopulation(pop_size, chromosome_l)
    fitnesses = map(fitnessF, init_pop)
    pop = copy.copy(init_pop)
    pool = multiprocessing.Pool()
    #fitnesses = pool.map(fitnessF, init_pop)
    for iteration in range(iterations):
        new_pop = []
        for i in range(len(init_pop)/2):
            a = selection(pop, fitnesses,3)[0]
            b = selection(pop, fitnesses,3)[0]
            if random.uniform(0,1) < p_crossover:
                [a,b] = crossoverOnePoint([a,b])
            new_pop.append(a)
            new_pop.append(b)
        for i in range(len(new_pop)):
            new_pop[i] = mutation(new_pop[i], p_mutation)
        fitnesses = map(fitnessF, new_pop)
        #fitnesses = pool.map(fitnessF, new_pop)
        pop = new_pop
    return pop


fitness = oneMax

result = geneticAlgorithm(fitness, 1000, 100, 100, 0.6, 0.01)
bestOne = result[0]

for i in range(len(result)):
    if fitness(result[i]) > fitness(bestOne):
        bestOne = result[i]
print(bestOne, fitness(bestOne))
