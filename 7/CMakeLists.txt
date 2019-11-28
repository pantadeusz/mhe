cmake_minimum_required(VERSION 3.5)

project(metaheuristics LANGUAGES CXX VERSION 0.0.1)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -fopenmp")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty")
file(GLOB_RECURSE app_SOURCES "src/*.cpp" "src/*/*.cpp")
# file(GLOB_RECURSE test_SOURCES "test/*.cpp")

add_executable(salesman ${app_SOURCES})
add_executable(salesman_dbg ${app_SOURCES})
add_executable(salesman_opt ${app_SOURCES})

set_target_properties(salesman PROPERTIES COMPILE_FLAGS "" )
set_target_properties(salesman_dbg PROPERTIES COMPILE_FLAGS "-ggdb" )
set_target_properties(salesman_opt PROPERTIES COMPILE_FLAGS "-O3" )

