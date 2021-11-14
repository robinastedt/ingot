find_package(LLVM REQUIRED)

add_library(ingotcodegen STATIC
    CodegenVisitor.cc
    Generator.cc
    List.cc
)

add_library(ingotruntime STATIC
    List.cc
)

target_compile_definitions(ingotruntime PRIVATE INGOT_RUNTIME=1)