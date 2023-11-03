cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(sort)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable (sort main.cpp fmt.cpp)
add_executable (test test.cpp)
