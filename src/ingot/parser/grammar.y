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
%locations

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
    #define yylex(val, loc) scanner->lex(val, loc)
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
%token <int64_t>            TYPE_INT
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

fundef      : IDENT ASSIGN fun         { $$ = ast::FunctionDefinition{$1, $3}; $$.setLocation(@1 + @3);}

fun         : funtype COLON LPAREN args RPAREN RARROW expr  { $$ = ast::Function($1, $4, $7); $$.setLocation(@1 + @7); }

funtype     : LPAREN argtypes RPAREN RARROW type  { $$ = ast::FunctionType($5, $2); $$.setLocation(@1 + @5);  }

args            : %empty                    { $$ = std::vector<std::string>(); }
                | IDENT                     { $$ = {$1}; }
                | args COMMA IDENT          { std::vector<std::string> copy = $1; copy.push_back($3); $$ = std::move(copy); }

argtypes        : %empty                    { $$ = std::vector<ast::Type>(); }
                | type                      { $$ = {$1}; }
                | argtypes COMMA type       { std::vector<ast::Type> copy = $1; copy.push_back($3); $$ = std::move(copy); }

exprs           : %empty                    { $$ = std::vector<ast::Expression>(); }
                | expr                      { $$ = {$1}; }
                | exprs COMMA expr          { std::vector<ast::Expression> copy = $1; copy.push_back($3); $$ = std::move(copy); }

expr    : INTEGER                   { $$ = ast::Integer($1); $$.setLocation(@1); }
        | STRING                    { $$ = ast::String($1); $$.setLocation(@1); }
        | IDENT LPAREN exprs RPAREN { $$ = ast::FunctionCall($1, $3); $$.setLocation(@1 + @4); }
        | IDENT                     { $$ = ast::ArgumentReference($1); $$.setLocation(@1); }
        | expr PLUS expr            { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Add); $$.setLocation(@1 + @3); }
        | expr MINUS expr           { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Sub); $$.setLocation(@1 + @3); }
        | expr MULTIPLY expr        { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Mul); $$.setLocation(@1 + @3); }
        | expr DIVIDE expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Div); $$.setLocation(@1 + @3); }
        | expr MODULO expr          { $$ = ast::Operator(std::make_unique<ast::Expression>($1), std::make_unique<ast::Expression>($3), ast::Operator::Variant::Mod); $$.setLocation(@1 + @3); }
        | MINUS expr %prec UMINUS   { $$ = ast::Operator(std::make_unique<ast::Expression>(ast::Integer(0)), std::make_unique<ast::Expression>($2), ast::Operator::Variant::Sub); $$.setLocation(@1 + @2); }
        | LPAREN expr RPAREN        { $$ = $2;  $$.setLocation(@2); }
        ;

type    : TYPE_INT                 { $$ = ast::Type::integer($1); $$.setLocation(@1); }
        | LBRACKET type RBRACKET    { $$ = ast::Type::list($2); $$.setLocation(@1 + @3); }
%%
 

void
ingot::parser::Parser::error(const ingot::parser::location& location, const std::string& msg) {
    throw SyntaxError(msg + ": unrecognized or unexpected token '" + scanner->YYText() + "'", location);
}
