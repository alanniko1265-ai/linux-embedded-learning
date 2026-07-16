cmake_minimum_required(VERSION 3.16)

project(day04_cmake_basic VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(calc_tool
    src/main.c
    src/calc.c
)

target_include_directories(calc_tool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(calc_tool PRIVATE
    -Wall
    -g
    -O0
)
