find_package(LLVM REQUIRED CONFIG)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(COMPILER_TARGET ingotc)
set(CODEGEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/codegen)

make_directory(${CODEGEN_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CODEGEN_DIR}
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
