#!/bin/python

import numpy
from scipy.optimize import least_square
import matplotlib.pyplot

# From variant
x = linspace(0.1, 10, 1000)
stdev = 0.2
A,B,C = 1, 1, 1
def f(params, x):
    A,B,C = params
    return A*numpy.log(B*x + C)

y_true = f([A, B, C])

X_rand = x + numpy.random.normal(scale=stdev)
Y_rand = y_true + numpy.random.normal(scale=stdev) # ???

def zel(params, x, y):
    return y - f(params,x)

ini = [1.0,1.0,1.0] # TODO select right

result = least_squares(zel, ini, args=(X_rand,Y_rand))
A_m,B_m,C_m = result.x

x_m = numpy.linspace(numpy.min(X_rand), numpy.max(X_rand))
y_m = f([A_m,B_m,C_m], x)

# TODO matplot *_m

