
#include <iostream>
#include <FlexLexer.h>
#include <parser/Scanner.hh>
#include <parser/SyntaxError.hh>
#include <ast/AST.hh>
#include <semantics/SemanticTree.hh>
#include <codegen/Generator.hh>

int main() {
    ingot::parser::Scanner scanner{ std::cin, std::cerr };
    ingot::ast::AST ast;
    ingot::parser::Parser parser{ &scanner, ast };
    std::cout.precision(10);
    try {
        parser.parse();
    } catch (ingot::parser::SyntaxError err) {
        std::cerr << "example.ingot:" << err.lineno() << ":" << err.colno() << ": " << err.what() << std::endl;
        return 1;
    }
    
    std::cout << ast << std::endl
              << "++++++++++++++++++++++++" << std::endl;
    ingot::codegen::Generator generator;
    ingot::semantics::SemanticTree semTree{ast};
    if (!semTree.verify(std::cerr)) {
        return 1;
    }

    generator.run(semTree);
}