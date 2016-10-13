cmake_minimum_required(VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(LibGrassTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
link_directories(${PROJECT_SOURCE_DIR}/lib/)

set(SOURCE_FILES
    LibGrass.h
    test.cpp)

add_executable(test_grass ${SOURCE_FILES})
target_link_libraries(test_grass grass)