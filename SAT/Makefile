CC=clang
CXX=clang++
CC_FLAGS= --std=c++2b  --stdlib=libc++ -fmodules -O2
BUILD ?= build

define generate_module
--precompile $(1).cppm -o $(BUILD)/$(1).pcm
endef 
define use_module
-fmodule-file=$(BUILD)/$(1).pcm $(BUILD)/$(1).pcm
endef 
define use_precomp_module
-fmodule-file=$(BUILD)/$(1).pcm
endef 

all: main test

Common: Common.cppm
	$(CXX) $(CC_FLAGS) $(call generate_module,$@)
Sat: Common Sat.cppm
	$(CXX) $(CC_FLAGS) $(call use_precomp_module,Common) $(call generate_module,$@)
Algo: Sat Algo.cppm
	$(CXX) $(CC_FLAGS) $(call use_precomp_module,Sat) $(call generate_module,$@)

main: Common Sat Algo main.cpp
	$(CXX) $(CC_FLAGS) $(call use_module,Common) $(call use_module,Sat) $(call use_module,Algo) $@.cpp -o $(BUILD)/$@.out

test: Common Sat Algo test.cpp
	$(CXX) $(CC_FLAGS) $(call use_module,Common) $(call use_module,Sat) $(call use_module,Algo) $@.cpp -o $(BUILD)/$@.out

clean: 
	rm $(BUILD)/*.out $(BUILD)/*.pcm

format:
	find ./ -name '*.cppm' -exec clang-format -i -style=llvm {} \;
	find ./ -name '*.cpp' -exec clang-format -i -style=llvm {} \;
	find ./ -name '*.h' -exec clang-format -i -style=llvm {} \;