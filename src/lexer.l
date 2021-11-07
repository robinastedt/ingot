%{
#include <cstdlib>
#include "Parser.hh"
%}

%option reentrant interactive noyywrap noyylineno nodefault outfile="Scanner.cc" header="Scanner.hh"

dseq            ([[:digit:]]+)
dseq_opt        ({dseq}?)
frac            (({dseq_opt}"."{dseq})|{dseq}".")
exp             ([eE][+-]?{dseq})
exp_opt         ({exp}?)
integer         ({dseq})
float           (({frac}{exp_opt})|({dseq}{exp}))
intvar          ([[:upper:]])
fltvar          ([[:lower:]])

%%
 
{integer}   yylval->emplace<long long>(strtoll(yytext, nullptr, 10)); return ingot::Parser::token::INT;
{float}     yylval->emplace<double>(strtod(yytext, nullptr)); return ingot::Parser::token::FLT;
{intvar}    yylval->emplace<char>(yytext[0]); return ingot::Parser::token::INTVAR;
{fltvar}    yylval->emplace<char>(yytext[0]); return ingot::Parser::token::FLTVAR;
"+"         return ingot::Parser::token::PLUS;
"-"         return ingot::Parser::token::MINUS;
"*"         return ingot::Parser::token::MULTIPLY;
"/"         return ingot::Parser::token::DIVIDE;
"%"         return ingot::Parser::token::MODULO;
"!"         return ingot::Parser::token::FACTORIAL;
"^"         return ingot::Parser::token::EXPONENT;
"("         return ingot::Parser::token::LPAREN;
")"         return ingot::Parser::token::RPAREN;
"="         return ingot::Parser::token::ASSIGN;
\n          return ingot::Parser::token::EOL;
<<EOF>>     return ingot::Parser::token::YYEOF;
.           /* no action on unmatched input */

%%
/*
int main() {
    yyscan_t scanner;
    yylex_init(&scanner);
    ingot::Parser parser{ scanner };
    std::cout.precision(10);
    parser.parse();
    yylex_destroy(scanner);
}
*/