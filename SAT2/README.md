# Prerequests

* Download clang-17 using [link](https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/).
* Download clang++, abi support, clang-tools-17
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