find_package(LLVM REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(COMPILER_TARGET ingotc)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # Needed for generated parser
)

add_executable(${COMPILER_TARGET}
    main.cc
)

target_link_libraries(${COMPILER_TARGET}
    ingotparser
    ingotast
    LLVM
)

add_subdirectory(parser)
add_subdirectory(ast)
