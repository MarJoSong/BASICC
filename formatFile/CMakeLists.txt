cmake_minimum_required(VERSION 3.10)
project(formatFile C)

set(CMAKE_C_STANDARD 11)


file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
    target_link_libraries(${name} pthread)
endforeach()