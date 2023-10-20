# How to use modules with CMake

* Download clang-17 using [link](https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/).
* Download clang++, abi support, clang-tools-17
* Download cmake-2.38+ using [link](https://cmake.org/download/) in binary distrubutions
* Donwload ninja-1.11+ using [link](https://github.com/ninja-build/ninja/releases)

Finally, run
```bash
CXX=clang++-17 CC=clang-17 /full/path/to/cmake-3.28 -GNinja -DCMAKE_MAKE_PROGRAM=/full/path/to/ninja -S . -B build
/full/path/to/cmake-3.28 --build build
```