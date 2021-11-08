
#include <iostream>
#include <FlexLexer.h>
#include <parser/Scanner.hh>
#include <ast/AST.hh>

int main() {
    ingot::Scanner scanner{ std::cin, std::cerr };
    ingot::ast::AST ast;
    ingot::Parser parser{ &scanner, ast };
    std::cout.precision(10);
    parser.parse();
    std::cout << ast;
}