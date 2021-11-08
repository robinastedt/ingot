%{
#include <parser/Parser.hh>
#include <parser/Scanner.hh>

#define YY_DECL int ingot::Scanner::lex(ingot::Parser::semantic_type *yylval)
%}

%option c++ interactive noyywrap noyylineno nodefault outfile="Scanner.cc"

/*
dseq            ([[:digit:]]+)
dseq_opt        ({dseq}?)
frac            (({dseq_opt}"."{dseq})|{dseq}".")
exp             ([eE][+-]?{dseq})
exp_opt         ({exp}?)
*/

integer         ([[:digit:]]+)

ident           ([a-zA-Z]+[a-zA-Z0-9]*)
/*float           (({frac}{exp_opt})|({dseq}{exp}))*/

whitespace      ([ \t\n]+)

%x COMMENT

%%

"#"                 { BEGIN(COMMENT); }
<COMMENT>"\n"       { BEGIN(INITIAL); }
<COMMENT><<EOF>>    { BEGIN(INITIAL); return Parser::token::YYEOF; }
<COMMENT>.          { }
{whitespace}        { }
{integer}           { yylval->emplace<int64_t>(strtoll(YYText(), nullptr, 10)); return Parser::token::INTEGER; }
{ident}             { yylval->emplace<std::string>(YYText()); return Parser::token::IDENT; }
"+"                 { return Parser::token::PLUS; }
"-"                 { return Parser::token::MINUS; }
"*"                 { return Parser::token::MULTIPLY; }
"/"                 { return Parser::token::DIVIDE; }
"%"                 { return Parser::token::MODULO; }
"("                 { return Parser::token::LPAREN; }
")"                 { return Parser::token::RPAREN; }
"="                 { return Parser::token::ASSIGN; }
<<EOF>>             { return Parser::token::YYEOF; }

%%

int yyFlexLexer::yylex() {
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}
