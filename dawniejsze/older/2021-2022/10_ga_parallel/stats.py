
import numpy as np
from numpy import genfromtxt


a = genfromtxt('summary.csv', delimiter=' ')
means = np.mean(a, axis=0)

print(means)