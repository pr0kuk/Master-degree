cmake_minimum_required(VERSION 3.10)
project(Sat)

find_package(GTest REQUIRED)

set(CMAKE_C_COMPILER "gcc-13")
set(CMAKE_CXX_COMPILER "g++-13")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3") # use CMAKE_CXX_STANDARD
set(CMAKE_CXX_STANDARD 20)

add_executable( main main.cpp Sat.cpp )
add_executable( test test.cpp Sat.cpp )
target_link_libraries( test GTest::gtest_main )

set(BINARIES
  main
  test
)

add_custom_target(format
  DEPENDS format_depends
)
add_custom_target(binaries
  DEPENDS ${BINARIES}
)

add_custom_command(OUTPUT format_depends
  COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/format.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Formating files"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)