
#include <iostream>
#include <Scanner.hh>
#include <Parser.hh>

int main() {
    yyscan_t scanner;
    yylex_init(&scanner);
    ingot::Parser parser{ scanner };
    std::cout.precision(10);
    parser.parse();
    yylex_destroy(scanner);
}