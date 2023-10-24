import sys
import os
import subprocess
flags = {'-dir': [False, []], '-o': [False, []], '-file': [False, []]}

# parsing arguments
for a in sys.argv[1:]:
    for arg, [flag, lst] in flags.items():
        if a[0] == '-':
            flags[arg][0] = False if a != arg else True
        elif flag == True:
            flags[arg][1].append(a)
dir_path = os.path.dirname(os.path.realpath(__file__)) 

for d, i in flags['-dir'][1], range(len(flags['-dir'][1])):
    dir_list = os.listdir(d)
    o = '' if i >= len(flags['-o'][1]) else flags['-o'][1][i]

    # calling bench.cxx
    for f in dir_list:
        proc = subprocess.call([os.path.join(dir_path,'build', 'bench'), os.path.join(dir_path,d,f), os.path.join(dir_path,o) if o!='' else o])

    # reading results
    if o != '':
        with open(o) as out:
            s = out.readlines().split()
            mean = s[s.index('Mean:')+1]
            print(mean)

    #plotting graphs