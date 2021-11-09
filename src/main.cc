
#include <iostream>
#include <FlexLexer.h>
#include <parser/Scanner.hh>
#include <ast/AST.hh>
#include <semantics/SemanticTree.hh>
#include <codegen/Generator.hh>

int main() {
    ingot::Scanner scanner{ std::cin, std::cerr };
    ingot::ast::AST ast;
    ingot::Parser parser{ &scanner, ast };
    std::cout.precision(10);
    parser.parse();
    std::cout << ast << std::endl
              << "++++++++++++++++++++++++" << std::endl;
    ingot::codegen::Generator generator;
    ingot::semantics::SemanticTree semTree{ast};
    if (!semTree.verify(std::cerr)) {
        return 1;
    }

    generator.run(semTree);
}