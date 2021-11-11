cmake_minimum_required(VERSION 3.20)

project(ingot CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
