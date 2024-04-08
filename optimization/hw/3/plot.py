import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

def f1(x):
    return -1 + x

def f2(x):
    return -1 - 2 * x

def f3(x):
    return -x / 2

def f4(x):
    return x / 2

def f(x):
    return max(f1(x), max(f2(x), max(f3(x), f4(x))))

xs = np.linspace(-5, 6, 10000)
ys = []
for x in xs:
    ys += [f(x)]

fig, ax = plt.subplots()

ax.plot(xs, ys)

xy = [[0, 0], [2, 1], [6, 5], [6, 9], [-5, 9], [-2/3, 1/3], [0, 0]]

p = Polygon(xy, facecolor='b')

ax.add_patch(p)

plt.savefig('plot_.png')
