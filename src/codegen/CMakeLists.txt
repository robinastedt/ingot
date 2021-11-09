find_package(LLVM REQUIRED)

add_library(ingotcodegen STATIC
    CodegenVisitor.cc
    Generator.cc
)