import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from data.cc_set_insert_randomized_runtime import x as x_blist
from data.cc_set_insert_randomized_runtime import y as y_blist
from data.std_set_insert_randomized_runtime import x as x_std
from data.std_set_insert_randomized_runtime import y as y_std

def divide_n(x, y):
    for i in range(0, len(x)):
        y[i] /= x[i]

divide_n(x_blist, y_blist)
divide_n(x_std, y_std)

x_diff = x_blist
y_diff = []
for i in range(0, len(x_diff)):
    y_diff.append(y_std[i] - y_blist[i])

fig = plt.figure()
ax = fig.add_axes([0.15, 0.1, 0.8, 0.8])
ax.plot(x_blist, y_blist, "C0H-")
ax.plot(x_std, y_std, "C1H-")
ax.plot(x_diff, y_diff, "C2H-")
ax.set_title('Runtime cost of randomized set insertions (AMD64 Zen-2)')
ax.set_xlabel('Number of elements')
ax.set_ylabel('Duration [us]')
ax.legend(labels = ('a: blist_set<int>::insert()', 'b: std::set<int>::insert()', 'b - a'), loc = 'upper left')

plt.savefig("plots/set_insert_randomized_plot.svg")
