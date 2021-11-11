
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # Needed for generated parser
)

add_executable(ingotc
    main.cc
)

target_link_libraries(ingotc
    ingotparser
    ingotast
    ingotsemantics
    ingotcodegen
    LLVM
)

add_test(
    NAME smoke_test
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ingotc ${CMAKE_SOURCE_DIR}/example.ingot
)

add_subdirectory(parser)
add_subdirectory(ast)
add_subdirectory(semantics)
add_subdirectory(codegen)
