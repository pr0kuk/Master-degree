cmake_minimum_required(VERSION 3.28)

add_library(testCross)
target_sources(testCross
  PUBLIC
    FILE_SET CXX_MODULES FILES
    testData.cxx 
)

add_library(SatCross)
target_sources(SatCross
  PUBLIC
    FILE_SET CXX_MODULES FILES
    Sat.cxx 
    testData.cxx
)

add_executable(testCross test.cxx)
target_link_libraries(testCross SatCross)

install(TARGETS testCross DESTINATION .)