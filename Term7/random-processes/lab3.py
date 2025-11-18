#!/bin/python

import numpy
from scipy.optimize import least_squares
import matplotlib.pyplot as plt

# From variant
x = numpy.linspace(0.1, 10, 1000)
stdev = 0.2
A,B,C = 1, 1, 1
def f(params, x):
    A,B,C = params
    return A*numpy.log(B*x + C)

y_true = f([A, B, C], x)

X_rand = x + numpy.random.normal(scale=stdev, size=(x.size,))
Y_rand = y_true + numpy.random.normal(scale=stdev, size=(y_true.size,))

def zel(params, x, y):
    return y - f(params,x)

ini = [1.0,1.0,1.0]

result = least_squares(zel, ini, args=(X_rand,Y_rand))
A_m,B_m,C_m = result.x

x_m = numpy.linspace(numpy.min(X_rand), numpy.max(X_rand))
y_m = f([A_m,B_m,C_m], x)
print(A_m, B_m, C_m)

fig, ax = plt.subplots()

ax.plot(x, y_m, label='Approximation', markeredgewidth=2)
ax.plot(x, y_true, label='True function', markeredgewidth=2)
ax.scatter(X_rand, Y_rand, s=1, zorder=2, label="Function")

ax.set(xlim=(0, 10),
       ylim=(0, 20))
ax.legend()
ax.set_xlabel("x")
ax.set_ylabel("y")

plt.tight_layout()
plt.show()

