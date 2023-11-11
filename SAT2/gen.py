import numpy as np
import random
import argparse

# parsing arguments
parser = argparse.ArgumentParser()
parser.add_argument('-v', help='Number of variables', nargs=1, metavar='VarCount', default = [0] ,type = int)
parser.add_argument('-c', help = 'Number of clauses', nargs=1, metavar='ClauseCount', default = [0], type = int)
parser.add_argument('-o', help = 'Output file', nargs=1, metavar='Out', required = True, type = str)
parser.add_argument('-n', help = 'Number of tests', nargs=1, metavar='N', default = [0], type = int)
args = parser.parse_args()

# initializing parameters
VarCount = 10 if args.v[0] == 0 else args.v[0]
ClauseCount = 20 if args.c[0] == 0 else args.c[0]
N = 1 if args.n[0] == 0 else args.n[0]
if VarCount < 3:
    print("Number of variables should be greater than 2")
    quit()
if ClauseCount < 1:
    print("Number of clauses should be greater than 0")
    quit()
if N < 1:
    print("Number of tests should be greater than 0")
    quit()

# reading template
with open("gendata_template.txt", "r") as f:
    s = f.read()


for n in range(N):
    vars = np.arange(1,VarCount+1)
    clauses = []

    # generating clauses
    for i in range(ClauseCount):
        clause = []
        for j in range(3):
            c = random.randint(1, VarCount+1)
            while (c in clause):
                c = random.randint(1, VarCount+1)
            clause.append(c)
        for c, j in zip(clause, range(3)):
            clause[j] = c if random.randint(0, 1) == 0 else -c
        clauses.append(clause)

    # storing results into str
    s += "\n\nexport constexpr Test_t<"+str(VarCount)+", "+str(ClauseCount)+"> genTest"+ str(n)+"(std::array<std::array<int, 3>, "+str(ClauseCount)+">({\n\t"
    for c, j in zip(clauses, range(ClauseCount)):
        s += "{"
        for i in range(3):
            s += str(c[i])
            if i != 2:
                s += ", "
        s += "}"
        if j != ClauseCount - 1:
            s+=", "
        if j % 4 == 0 and j != 0:
            s += "\n\t"
    s += "}));"

# storing results into file
with open(args.o[0], "w") as f:
    f.write(s)