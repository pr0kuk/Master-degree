# Prerequests

* Download clang-17 using [link](https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/).
* Download clang++, abi support, clang-tools-17
* Download python3
* Download python libs: argparse, os, numpy, pyplot, subprocess
* Download cmake-2.38+ using [link](https://cmake.org/download/) in binary distrubutions
* Donwload ninja-1.11+ using [link](https://github.com/ninja-build/ninja/releases)

# Build project
* Ether set env variables **CMAKE** and **NINJA** with /full/path to binaries or add directory to **PATH** 
* Also you can set **BUILD** env variable to specify path for binaries (default is *build*)
* Run **make**
* Test binary in *BUILD* folder.

Example:
```bash
CMAKE=/.../cmake-3.28.0-rc2-linux-x86_64/bin/cmake NINJA=/.../ninja make
./build/test
```

# Usage examples
## Prepairing
Before running the benchmarks be sure that the project is successfully built.  
To run tests:
```bash
build/test
```
You'll need to download third-party benchmarks input files meet the [CNF Format](https://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/satformat.ps).  
For example from [here](https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html).

## Running the benchmarks and plotting graphs
This example will store all results of running every algorithm on every file from `/PATH/TO/uf20_91/` in `uf20_91.txt` and then build plots based on it.
```bash
python3 benchmark.py -dir /PATH/TO/uf20_91 -o uf20_91.txt
```

## Just plotting graphs
This example will read results from `uf20_91.txt` and then build plots based on it.
```bash
python3 benchmark.py -o uf20_91.txt
```