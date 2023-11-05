cmake_minimum_required(VERSION 3.28)

add_library(testData)
target_sources(testData
  PUBLIC
    FILE_SET CXX_MODULES FILES
      testData.cxx 
)

add_library(Sat1Compile)
target_sources(Sat1Compile
  PUBLIC
    FILE_SET CXX_MODULES FILES
    Sat1.cxx 
    testData.cxx
)

add_executable(testCompile test.cxx)
target_link_libraries(testCompile Sat1Compile)

install(TARGETS testCompile DESTINATION .)