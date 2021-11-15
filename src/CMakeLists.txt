
cmake_minimum_required(VERSION 3.20)
project(ingot CXX)
enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # Needed for generated parser
)

add_subdirectory(ingot)