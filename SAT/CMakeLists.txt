cmake_minimum_required(VERSION 3.10)
project(Sat)

find_package(GTest REQUIRED)

set(CMAKE_C_COMPILER "gcc-13")
set(CMAKE_CXX_COMPILER "g++-13")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3") # use CMAKE_CXX_STANDARD
set(CMAKE_CXX_STANDARD 20)

add_executable(a.out main.cpp)

add_executable(
  my_test
  test.cpp
)
target_link_libraries(
  my_test
  GTest::gtest_main
)