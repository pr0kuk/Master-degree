cmake_minimum_required(VERSION 3.28)
project(std_module_example CXX)

# Turning off extensions avoids and issue with the clang 16 compiler
# clang 17 and greater can avoid this setting
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the version of C++ for the project
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

# Create an executable
add_executable(test test.cxx)
add_executable(bench bench.cxx)

add_library(Common)
target_sources(Common
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Common.cxx 
)

foreach(NUM RANGE 1 4)
  add_library(Sat${NUM})
  target_sources(Sat${NUM}
    PUBLIC
      FILE_SET CXX_MODULES FILES
        Sat${NUM}.cxx
        BaseSat.cxx
  )
  target_link_libraries(test Sat${NUM})
  target_link_libraries(bench Sat${NUM})
endforeach()

# Link to dependencies
target_link_libraries(test Common)
target_link_libraries(bench Common Boost::program_options)