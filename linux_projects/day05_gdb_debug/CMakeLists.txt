cmake_minimum_required(VERSION 3.16)

project(day05_gdb_debug VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(debug_demo
    src/main.c
    src/device.c
)

target_include_directories(debug_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(debug_demo PRIVATE
    -Wall
    -g
    -O0
)
