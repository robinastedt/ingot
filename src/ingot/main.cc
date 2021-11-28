
#include <iostream>
#include <fstream>
#include <FlexLexer.h>
#include <ingot/parser/Scanner.hh>
#include <ingot/parser/SyntaxError.hh>
#include <ingot/ast/AST.hh>
#include <ingot/semantics/SemanticTree.hh>
#include <ingot/semantics/SemanticError.hh>
#include <ingot/codegen/Generator.hh>

constexpr bool DEBUG = false;

int main(int argc, char** argv) {
    std::string filename;
    std::ifstream fileStream;
    if (argc > 1) {
        filename = argv[1];
        fileStream = std::ifstream{filename};
    }
    std::istream& inputStream = filename.empty() ? std::cin : fileStream;
    ingot::parser::Scanner scanner{ inputStream, std::cerr };
    ingot::ast::AST ast;
    ingot::parser::Parser parser{ &scanner, ast };
    try {
        parser.parse();
    } catch (const ingot::parser::SyntaxError& err) {
        std::cerr << filename << ":" << err.lineno() << ":" << err.colno() << ": " << err.what() << std::endl;
        return 1;
    }

    //std::cout << ast << std::endl
    //          << "++++++++++++++++++++++" << std::endl;
    
    ingot::semantics::SemanticTree semTree{ast};
    try {
        semTree.resolve();
    } catch (const ingot::semantics::SemanticError& err) {
        if constexpr (DEBUG) {
            std::cerr << semTree << std::endl;
        }
        std::cerr << filename << ":" << err.lineno() << ":" << err.colno() << ": " << err.what() << std::endl;
        return 1;
    }
    if constexpr (DEBUG) {
        std::cerr << semTree << std::endl;
    }
    ingot::codegen::Generator generator;
    generator.run(semTree);
}