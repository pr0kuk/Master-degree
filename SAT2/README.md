# Prerequests

* Download clang-17 using [link](https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/).
* Download clang++, abi support, clang-tools-17
* Download cmake-2.38+ using [link](https://cmake.org/download/) in binary distrubutions
* Donwload ninja-1.11+ using [link](https://github.com/ninja-build/ninja/releases)

# Build project
* In current repository create file **MyMakefile** and set there variables **CMAKE** and **NINJA** with /full/path to binaries
* Run **make**