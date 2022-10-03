import numpy as np
import sys

data = np.loadtxt(sys.argv[1])
print( data.mean(axis=0)[0], data.mean(axis=0)[1])
