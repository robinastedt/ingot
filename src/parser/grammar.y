%{
#include <iostream>
#include <parser/SyntaxError.hh>
%}
 
%require "3.7.4"
%language "C++"
%defines "Parser.hh"
%output "Parser.cc"
 
%define api.parser.class {Parser}
%define api.namespace {ingot::parser}
%define api.value.type variant
%parse-param {Scanner* scanner} {ast::AST& outputAST}
 
%code requires
{
    #include <ast/AST.hh>
    #include <ast/Expression.hh>
    #include <ast/FunctionDefinition.hh>
    #include <string>

    namespace ingot::parser {
        class Scanner;
    }
    
} // %code requires
 
%code
{   
    #include <FlexLexer.h>
    #include <parser/Scanner.hh>
    #define yylex(x) scanner->lex(x)
}
 
%token                      LPAREN RPAREN
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
module  : %empty
        | module fundef             { outputAST.addDefinition($2); }
        | module error '\n'         { yyerrok; }
        ;

fundef  : IDENT ASSIGN expr         { $$ = ast::FunctionDefinition{ast::FunctionPrototype{$1}, $3}; }

expr    : INTEGER                   { $$ = ast::Integer($1); }
        //| FLOAT                     { $$ = $1; }
        | IDENT                     { $$ = ast::FunctionCall($1); }
        | expr PLUS expr            { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Type::Add); }
        | expr MINUS expr           { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Type::Sub); }
        | expr MULTIPLY expr        { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Type::Mul); }
        | expr DIVIDE expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Type::Div); }
        | expr MODULO expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Type::Mod); }
        | MINUS expr %prec UMINUS   { $$ = ast::Operator(std::make_unique<ast::Expression>(ast::Integer(0)), std::make_unique<ast::Expression>($2), ast::Operator::Type::Sub);; }
        | LPAREN expr RPAREN        { $$ = $2; }
        ;

 
%%
 
void ingot::parser::Parser::error(const std::string& msg) {
    const int lineno = scanner->lineno();
    const int colno = scanner->getColumn() + 1 - scanner->YYLeng();
    throw SyntaxError(msg + ": unrecognized or unexpected token \"" + scanner->YYText() + "\"", lineno, colno);
}