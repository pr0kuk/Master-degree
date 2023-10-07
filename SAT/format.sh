find ./ -name '*.cpp' -exec clang-format -i -style=llvm {} \;
find ./ -name '*.h' -exec clang-format -i -style=llvm {} \;