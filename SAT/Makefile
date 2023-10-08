CC=clang
CXX=clang++
CC_FLAGS= --std=c++2b  --stdlib=libc++ -fmodules -O2
CC_GTEST_FLAGS= -lgtest_main -lgtest -lpthread 

define generate_module
--precompile $(1).cppm -o $(1).pcm
endef 
define use_module
-fmodule-file=$(1).pcm $(1).pcm
endef 

# prepare first module
Sat: Sat.cppm
	$(CXX) $(CC_FLAGS) $(call generate_module,$@)


main: Sat main.cpp
	$(CXX) $(CC_FLAGS) $(call use_module,Sat) $@.cpp

test: Sat test.cpp
	$(CXX) $(CC_FLAGS) $(call use_module,Sat) $@.cpp $(CC_GTEST_FLAGS) 

all: main test