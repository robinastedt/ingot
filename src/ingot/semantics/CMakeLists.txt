add_library(ingotsemantics
    IdentifierResolver.cc
    SemanticError.cc
    SemanticTree.cc
    TypeResolver.cc
)

target_link_libraries(ingotsemantics
    ingotast
)