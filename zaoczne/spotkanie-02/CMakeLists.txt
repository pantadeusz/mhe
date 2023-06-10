cmake_minimum_required(VERSION 3.10)
project(mhe)

set(CMAKE_CXX_STANDARD 20)

add_executable(mhe main.cpp solution_t.cpp solution_t.h problem_t.h vec2d.h problem_t.cpp)
