find_package(LLVM REQUIRED)

add_library(ingotcodegen STATIC
    CodegenVisitor.cc
    Generator.cc
    InsertPointGuard.cc
    ListOperations.cc
    TypeContext.cc
)
