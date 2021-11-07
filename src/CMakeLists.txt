find_package(LLVM REQUIRED CONFIG)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(COMPILER_TARGET ingotc)
set(CODEGEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/codegen)

include_directories(
    ${CODEGEN_DIR}
)

add_executable(${COMPILER_TARGET}
    main.cc
    ${CODEGEN_DIR}/Scanner.cc
    ${CODEGEN_DIR}/Parser.cc
)

add_dependencies(${COMPILER_TARGET}
    FlexBison
)

target_link_libraries(${COMPILER_TARGET}
    LLVM
)

make_directory(${CODEGEN_DIR})

add_custom_command(
    OUTPUT ${CODEGEN_DIR}/Scanner.cc ${CODEGEN_DIR}/Scanner.hh
    COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
    WORKING_DIRECTORY ${CODEGEN_DIR}
)

add_custom_command(
    OUTPUT ${CODEGEN_DIR}/Parser.cc ${CODEGEN_DIR}/Parser.hh
    COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
    WORKING_DIRECTORY ${CODEGEN_DIR}
)

add_custom_target(FlexBison
    DEPENDS
    ${CODEGEN_DIR}/Parser.cc
    ${CODEGEN_DIR}/Parser.hh
    ${CODEGEN_DIR}/Scanner.cc
    ${CODEGEN_DIR}/Scanner.hh
)