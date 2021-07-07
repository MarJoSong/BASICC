cmake_minimum_required(VERSION 3.10)
project(Thread C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(tinycthread/include)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
foreach(file ${files})
    get_filename_component(name ${file} NAME)
    add_executable(${name} ${file} tinycthread/tinycthread.c)
    target_link_libraries(${name} pthread)
endforeach()