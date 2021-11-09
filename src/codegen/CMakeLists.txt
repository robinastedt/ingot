find_package(LLVM REQUIRED)

add_library(ingotcodegen STATIC
    Generator.cc
)