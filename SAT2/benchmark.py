import sys
import os
import subprocess
import numpy as np
import matplotlib.pyplot as plt
import argparse
dir_path = os.path.dirname(os.path.realpath(__file__)) 
Sats = ['Sat2_t', 'Sat3_t']
res = {}

# parsing arguments
parser = argparse.ArgumentParser()
parser.add_argument('-dir', help='directories with input files', nargs='*', metavar='DIRECTORY', action='extend')
parser.add_argument('-file', help = 'input files', nargs='*', metavar='FILE', action='extend')
parser.add_argument('-o', help = 'output files', nargs='*', metavar='OUT', action='extend')
args = parser.parse_args()

# clearing output files
if len(args.dir) > 0:
    for o in args.o:
        open(o, 'w').close()

for d, i in zip(args.dir, range(len(args.o))):
    # finding files in dir
    dir_list = os.listdir(d)
    o = '' if i >= len(args.o) else args.o[i]

    # calling bench.cxx
    if len(args.dir) > 0:
        for f in dir_list:
            proc = subprocess.call([os.path.join(dir_path,'build', 'bench'), '-t', os.path.join(dir_path,d,f), '-o', os.path.join(dir_path,o) if o!='' else o])

# reading results
for o in args.o:
    if o != '':
        with open(o,'r') as out:
            s = out.read().split()
            res[o] = {s:np.zeros(4) for s in Sats}
            for w, i in zip(s, range(len(s))):
                if s[i] in Sats:
                    res[o][w][0] += float(s[i+2]) # analyze time
                    res[o][w][1] += float(s[i+4]) # check time
                    res[o][w][2] += float(s[i+6]) # find time
                    res[o][w][3] += 1             # number of files
            for k in res[o].keys():
                res[o][k][:3] = np.divide(res[o][k][:3], res[o][k][3]) # averaging

#plotting graphs
measurements = [[m[i][1] for m in np.array(list(res.values()))] for i in Sats]
x = np.arange(len(res))  # the label locations
width = 0.25  # the width of the bars
multiplier = 0
fig, ax = plt.subplots(layout='constrained')
for attribute, measurement in zip(Sats, measurements) :
    offset = width * multiplier
    rects = ax.bar(x + offset, measurement, width, label=attribute)
    ax.bar_label(rects, padding=3)
    multiplier += 1
ax.set_ylabel('Time (ms)')
ax.set_title('Time of alogorithms')
ax.set_xticks(x + width, res.keys())
ax.legend(loc='upper left')
plt.show()