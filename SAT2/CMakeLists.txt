cmake_minimum_required(VERSION 3.28)
project(std_module_example CXX)

# Turning off extensions avoids and issue with the clang 16 compiler
# clang 17 and greater can avoid this setting
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the version of C++ for the project
set(CMAKE_CXX_STANDARD 20)
# Create a library

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_library(Common)
target_sources(Common
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Common.cxx
)

add_library(Sat)
target_sources(Sat
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Sat.cxx
      BaseSat.cxx 
)

add_library(Sat2)
target_sources(Sat2
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Sat2.cxx
      BaseSat.cxx 
)

add_library(Sat3)
target_sources(Sat3
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Sat3.cxx
      BaseSat.cxx 
)

# Create an executable
add_executable(test test.cxx)
add_executable(bench bench.cxx)

# Link to dependencies
target_link_libraries(test Sat Sat2 Sat3 Common)
target_link_libraries(bench Sat Sat2 Sat3 Common Boost::program_options)