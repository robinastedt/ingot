%{
#include <iostream>
#include <string>
#include <ast/Expression.hh>
#include <ast/FunctionDefinition.hh>


%}
 
%require "3.7.4"
%language "C++"
%defines "Parser.hh"
%output "Parser.cc"
 
%define api.parser.class {Parser}
%define api.namespace {ingot}
%define api.value.type variant
%parse-param {Scanner* scanner}
 
%code requires
{
    #include <ast/Expression.hh>
    #include <ast/FunctionDefinition.hh>
    
    namespace ingot {
        class Scanner;
    }
    
} // %code requires
 
%code
{   
    #include <FlexLexer.h>
    #include <Scanner.hh>
    #define yylex(x) scanner->lex(x)
}
 
%token                      EOL LPAREN RPAREN
%token <int64_t>            INTEGER
//%token <double>           FLOAT
%token <std::string>        STRING
%token <std::string>        IDENT
%token                      ASSIGN

%nterm <ast::FunctionDefinition>    fundef
%nterm <ast::Expression>            expr

%left                       PLUS MINUS
%left                       MULTIPLY DIVIDE MODULO
%precedence                 UMINUS
 
%%

lines   : %empty
        | lines line
        ;
 
line    : EOL                       { std::cerr << "Read an empty line.\n"; }
        | fundef EOL                { std::cout << $1 << "\n"; }
        | error EOL                 { yyerrok; }
        ;
 
fundef  : IDENT ASSIGN expr         { $$ = ast::FunctionDefinition{ast::FunctionPrototype{$1}, $3}; }

expr    : INTEGER                   { $$ = ast::Integer($1); }
        //| FLOAT                     { $$ = $1; }
        | expr PLUS expr            { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), '+'); }
        | expr MINUS expr           { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), '-'); }
        | expr MULTIPLY expr        { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), '*'); }
        | expr DIVIDE expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), '/'); }
        | expr MODULO expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), '%'); }
        | MINUS expr %prec UMINUS   { $$ = ast::Operator(std::make_unique<ast::Expression>(ast::Integer(0)), std::make_unique<ast::Expression>($2), '-');; }
        | LPAREN expr RPAREN        { $$ = $2; }
        ;

 
%%
 
void ingot::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}