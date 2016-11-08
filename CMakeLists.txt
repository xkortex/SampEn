cmake_minimum_required(VERSION 3.6)
project(sampen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES sampen-1.2.c)
add_executable(sampen ${SOURCE_FILES})