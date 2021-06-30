cmake_minimum_required(VERSION 3.19)
project(Thread C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(tinycthread/include)

add_executable(Thread main.c tinycthread/tinycthread.c)