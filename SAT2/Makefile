CC := clang-17
CXX := clang++-17

CMAKE ?= cmake
NINJA ?= ninja
BUILD ?= build
INSTALL ?= install

all: ${BUILD} COMPILE ${INSTALL}

define ReMkdir
	[ ! -d $(1) ] || rm -rf $(1)
	mkdir $(1)
endef

${BUILD}: CMakeLists.txt
	$(call ReMkdir,${BUILD})
	CXX=${CXX} CC={CC} ${CMAKE} -S . -B ${BUILD} -GNinja -DCMAKE_MAKE_PROGRAM=${NINJA} -DCMAKE_INSTALL_PREFIX=${INSTALL}

COMPILE: ${BUILD}
	${CMAKE} --build ${BUILD}

${INSTALL}: ${BUILD} COMPILE
	$(call ReMkdir,${INSTALL})
	${CMAKE} --build ${BUILD} --target install

format:
	find ./ -name '*.cxx' -not -path "./${BUILD}" -exec clang-format -i -style=llvm {} \;
	