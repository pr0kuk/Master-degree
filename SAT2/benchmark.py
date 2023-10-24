import sys
import os
import subprocess
flags = {'-dir': [False, []], '-o': [False, []], '-file': [False, []]}
for a in sys.argv[1:]:
    for arg, [flag, lst] in flags.items():
        if a[0] == '-':
            flags[arg][0] = False if a != arg else True
        elif flag == True:
            flags[arg][1].append(a)
dir_path = os.path.dirname(os.path.realpath(__file__)) 

#for o in flags['-o'][1]:
if flags['-o'][1].len() !=0:
    o = flags['-o'][1][0]
    with open(o) as out:
        for d in flags['-dir'][1]:
        dir_list = os.listdir(d)
        for f in dir_list:
        proc = subprocess.call([os.path.join(dir_path,'build', 'bench'), os.path.join(dir_path,d,f), os.path.join(dir_path,o)])
        s = o.readlines().split()
        mean = s[s.index('Mean:')+1]
        print(mean)