import sys
import os
import subprocess
import numpy as np
import matplotlib.pyplot as plt
flags = {'-dir': [False, []], '-o': [False, []], '-file': [False, []]}
Sats = ['Sat1_t', 'Sat2_t', 'Sat3_t']

# parsing arguments
for a in sys.argv[1:]:
    for arg, [flag, lst] in flags.items():
        if a[0] == '-':
            flags[arg][0] = False if a != arg else True
        elif flag == True:
            flags[arg][1].append(a)
dir_path = os.path.dirname(os.path.realpath(__file__)) 

# clearing output files
if len(flags['-dir'][1]) > 0:
    for o in flags['-o'][1]:
        open(o, 'w').close()

res = {}
for d, i in zip(flags['-dir'][1], range(len(flags['-dir'][1]))):
    dir_list = os.listdir(d)
    o = '' if i >= len(flags['-o'][1]) else flags['-o'][1][i]

    # calling bench.cxx
    if len(flags['-dir'][1]) > 0:
        for f in dir_list:
            proc = subprocess.call([os.path.join(dir_path,'build', 'bench'), os.path.join(dir_path,d,f), os.path.join(dir_path,o) if o!='' else o])

# reading results
for o in flags['-o'][1]:
    if o != '':
        with open(o,'r') as out:
            s = out.read().split()
            res[o] = {'Sat1_t':[0,0,0],'Sat2_t':[0,0,0], 'Sat3_t':[0,0,0]}
            for i in range(len(s)):
                for k in res[o].keys():
                    if s[i]==k:
                        res[o][s[i]][0] += float(s[i+2])
                        res[o][s[i]][1] += float(s[i+4])
                        res[o][s[i]][2] += 1
            for k in res[o].keys():
                res[o][k][0] /= res[o][k][2]
                res[o][k][1] /= res[o][k][2]

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