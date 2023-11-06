cmake_minimum_required(VERSION 3.28)
project(std_module_example CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_COMPILER "clang-17")
set(CMAKE_CXX_COMPILER "clang++-17")
set(CMAKE_BUILD_TYPE Debug)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "-fconstexpr-backtrace-limit=0 -fconstexpr-steps=4294967295 -fconstexpr-depth=4294967295")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fconstexpr-backtrace-limit=0 -fconstexpr-steps=4294967295 -fconstexpr-depth=4294967295")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "/constexpr:steps4294967295 /constexpr:backtrace0 /constexpr:depth4294967295")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 /constexpr:steps4294967295 /constexpr:backtrace0 /constexpr:depth4294967295")
endif()
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

# Create an executable
add_executable(test test.cxx)
add_executable(bench bench.cxx)

add_library(Common)
target_sources(Common
  PUBLIC
    FILE_SET CXX_MODULES FILES
      Common.cxx 
)
add_library(BaseSat)
target_sources(BaseSat
  PUBLIC
    FILE_SET CXX_MODULES FILES
      BaseSat.cxx 
)

# Link to dependencies
target_link_libraries(test BaseSat Common)
target_link_libraries(bench BaseSat Common Boost::program_options)

foreach(NUM RANGE 1 6)
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

install(TARGETS test DESTINATION .)
install(TARGETS bench DESTINATION .)

# COMPILE TIME

add_subdirectory(CompileTime)