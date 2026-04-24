cmake_minimum_required(VERSION 3.10)
project(problem_080)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Optimize without static linking to ensure portability on OJ environment
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(code main.cpp)
