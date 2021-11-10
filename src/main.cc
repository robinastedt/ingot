
#include <iostream>
#include <fstream>
#include <FlexLexer.h>
#include <parser/Scanner.hh>
#include <parser/SyntaxError.hh>
#include <ast/AST.hh>
#include <semantics/SemanticTree.hh>
#include <codegen/Generator.hh>


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
    } catch (ingot::parser::SyntaxError err) {
        std::cerr << filename << ":" << err.lineno() << ":" << err.colno() << ": " << err.what() << std::endl;
        return 1;
    }

    std::cout << ast << std::endl
              << "++++++++++++++++++++++" << std::endl;
    
    ingot::codegen::Generator generator;
    ingot::semantics::SemanticTree semTree{ast};
    if (!semTree.verify(std::cerr)) {
        return 1;
    }

    generator.run(semTree);
}