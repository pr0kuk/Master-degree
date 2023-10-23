cmake_minimum_required(VERSION 3.28)
project(std_module_example CXX)

# Turning off extensions avoids and issue with the clang 16 compiler
# clang 17 and greater can avoid this setting
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the version of C++ for the project
set(CMAKE_CXX_STANDARD 20)
# Create a library

add_library(Common)
target_sources(Common
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Common.cxx
)

add_library(BaseSat)
# Add the module file to the library
target_sources(BaseSat
  PUBLIC
    FILE_SET CXX_MODULES FILES
      BaseSat.cxx
)

add_library(Sat)
target_sources(Sat
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Sat.cxx
      BaseSat.cxx 
)

# Create an executable
add_executable(test test.cxx)
# Link to the library foo
target_link_libraries(test BaseSat Sat Common)