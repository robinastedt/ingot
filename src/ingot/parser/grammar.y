%{
#include <iostream>
#include <ingot/parser/SyntaxError.hh>
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
    #include <ingot/ast/AST.hh>
    #include <ingot/ast/Expression.hh>
    #include <ingot/ast/FunctionDefinition.hh>
    #include <ingot/ast/Function.hh>
    #include <ingot/ast/String.hh>
    #include <string>

    namespace ingot::parser {
        class Scanner;
    }
    
} // %code requires
 
%code
{   
    #include <FlexLexer.h>
    #include <ingot/parser/Scanner.hh>
    #define yylex(x) scanner->lex(x)
}
 
%token                      LPAREN RPAREN
%token <int64_t>            INTEGER
//%token <double>           FLOAT
%token <std::string>        STRING
%token <std::string>        IDENT
%token                      ASSIGN
%token                      COLON
%token                      RARROW
%token                      COMMA
%token                      LBRACKET
%token                      RBRACKET
%token                      TYPE_INT8
%token                      TYPE_INT64
%token <std::string>        ERROR


%nterm <ast::FunctionDefinition>        fundef
%nterm <ast::Expression>                expr
%nterm <std::vector<ast::Expression>>   exprs
%nterm <ast::Function>                  fun
%nterm <std::vector<std::string>>       args
%nterm <ast::FunctionType>              funtype
%nterm <std::vector<ast::Type>>         argtypes
%nterm <ast::Type>                      type

%left                       PLUS MINUS
%left                       MULTIPLY DIVIDE MODULO
%precedence                 UMINUS
 
%%
module  : %empty
        | module fundef             { outputAST.addDefinition($2); }
        | module error '\n'         { yyerrok; }
        ;

fundef      : IDENT ASSIGN fun         { $$ = ast::FunctionDefinition{$1, $3}; }

fun         : funtype COLON LPAREN args RPAREN RARROW expr  { $$ = ast::Function($1, $4, $7); }

funtype     : LPAREN argtypes RPAREN RARROW type  { $$ = ast::FunctionType($5, $2); }

args            : %empty                    { $$ = std::vector<std::string>(); }
                | IDENT                     { $$ = {$1}; }
                | args COMMA IDENT          { std::vector<std::string> copy = $1; copy.push_back($3); $$ = std::move(copy); }

argtypes        : %empty                    { $$ = std::vector<ast::Type>(); }
                | type                      { $$ = {$1}; }
                | argtypes COMMA type       { std::vector<ast::Type> copy = $1; copy.push_back($3); $$ = std::move(copy); }

exprs           : %empty                    { $$ = std::vector<ast::Expression>(); }
                | expr                      { $$ = {$1}; }
                | exprs COMMA expr          { std::vector<ast::Expression> copy = $1; copy.push_back($3); $$ = std::move(copy); }

expr    : INTEGER                   { $$ = ast::Integer($1); }
        | STRING                    { $$ = ast::String($1); }
        | IDENT LPAREN exprs RPAREN  { $$ = ast::FunctionCall($1, $3); }
        | expr PLUS expr            { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Add); }
        | expr MINUS expr           { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Sub); }
        | expr MULTIPLY expr        { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Mul); }
        | expr DIVIDE expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Div); }
        | expr MODULO expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Mod); }
        | MINUS expr %prec UMINUS   { $$ = ast::Operator(std::make_unique<ast::Expression>(ast::Integer(0)), std::make_unique<ast::Expression>($2), ast::Operator::Variant::Sub);; }
        | LPAREN expr RPAREN        { $$ = $2; }
        ;

type    : TYPE_INT8                 { $$ = ast::Type::int8(); }
        | TYPE_INT64                { $$ = ast::Type::int64(); }
        | LBRACKET type RBRACKET    { $$ = ast::Type::list($2); }
%%
 
void ingot::parser::Parser::error(const std::string& msg) {
    const int lineno = scanner->lineno();
    const int colno = scanner->getColumn() + 1 - scanner->YYLeng();
    throw SyntaxError(msg + ": unrecognized or unexpected token '" + scanner->YYText() + "'", lineno, colno);
}