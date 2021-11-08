set(OUTPUT_DIR ${CODEGEN_DIR}/parser)

make_directory(${OUTPUT_DIR})

add_library(ingotparser STATIC
    ${OUTPUT_DIR}/Scanner.cc
    ${OUTPUT_DIR}/Parser.cc
)

add_custom_command(
    OUTPUT ${OUTPUT_DIR}/Scanner.cc
    COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
    WORKING_DIRECTORY ${OUTPUT_DIR}
)

add_custom_command(
    OUTPUT ${OUTPUT_DIR}/Parser.cc ${OUTPUT_DIR}/Parser.hh
    COMMAND bison -Wall ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
    WORKING_DIRECTORY ${OUTPUT_DIR}
)
