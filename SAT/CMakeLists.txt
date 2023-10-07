cmake_minimum_required(VERSION 3.10)
project(Sat)

add_executable(a.out main.cpp)

set(CMAKE_C_COMPILER "gcc-13")
set(CMAKE_CXX_COMPILER "g++-13")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3") # use CMAKE_CXX_STANDARD
set(CMAKE_CXX_STANDARD 20)