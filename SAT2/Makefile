CC := clang-17
CXX := clang++-17

include MyMakefile
CMAKE ?= cmake
NINJA ?= ninja
BUILD ?= build

all: ${BUILD}
	${CMAKE} --build ${BUILD}

${BUILD}: CMakeLists.txt
	rm -rf {BUILD}
	CXX=${CXX} CC={CC} ${CMAKE} -S . -B ${BUILD} -GNinja -DCMAKE_MAKE_PROGRAM=${NINJA}

format:
	find ./ -name '*.cxx' -not -path "./{BUILD}" -exec clang-format -i -style=llvm {} \;
	