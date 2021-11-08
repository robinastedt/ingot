
add_library(ingotparser STATIC
    Scanner.cc
    Parser.cc
)

add_custom_command(
    OUTPUT Scanner.cc
    COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
)

add_custom_command(
    OUTPUT Parser.cc Parser.hh
    COMMAND bison -Wall ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
)
