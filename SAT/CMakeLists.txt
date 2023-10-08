cmake_minimum_required(VERSION 3.10)
project(Sat)

find_package(GTest REQUIRED)

#set(CMAKE_C_COMPILER "clang-17")
set(CMAKE_CXX_COMPILER "clang++-17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules --precompile") # use CMAKE_CXX_STANDARD
add_compile_options(-stdlib=libc++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
add_executable( main main.cpp Sat.cppm )
add_executable( test test.cpp Sat.cppm )
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