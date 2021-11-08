
#include <iostream>
#include <FlexLexer.h>
#include <parser/Scanner.hh>

int main() {
    ingot::Scanner scanner{ std::cin, std::cerr };
    ingot::Parser parser{ &scanner };
    std::cout.precision(10);
    parser.parse();
}