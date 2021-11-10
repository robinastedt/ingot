%{
#include <parser/Parser.hh>
#include <parser/Scanner.hh>

#define YY_DECL int ingot::parser::Scanner::lex(ingot::parser::Parser::semantic_type *yylval)
#define YY_USER_ACTION setColumn(getColumn() + YYLeng());
%}

%option c++ interactive noyywrap yylineno nodefault outfile="Scanner.cc"

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

whitespace      ([ \t]+)
linebreak       (\n+)
string         (\"(\\.|[^"\\])*\")

%x COMMENT

%%

"#"                 { BEGIN(COMMENT); }
<COMMENT>"\n"       { BEGIN(INITIAL); setColumn(0); }
<COMMENT><<EOF>>    { BEGIN(INITIAL); return Parser::token::YYEOF; }
<COMMENT>.          { }
{whitespace}        { }
{linebreak}         { setColumn(0); }
{integer}           { yylval->emplace<int64_t>(strtoll(YYText(), nullptr, 10)); return Parser::token::INTEGER; }
{ident}             { yylval->emplace<std::string>(YYText()); return Parser::token::IDENT; }
{string}            { yylval->emplace<std::string>(YYText()+1, YYText()+YYLeng() - 1); return Parser::token::STRING; }
"+"                 { return Parser::token::PLUS; }
"-"                 { return Parser::token::MINUS; }
"*"                 { return Parser::token::MULTIPLY; }
"/"                 { return Parser::token::DIVIDE; }
"%"                 { return Parser::token::MODULO; }
"("                 { return Parser::token::LPAREN; }
")"                 { return Parser::token::RPAREN; }
"="                 { return Parser::token::ASSIGN; }
":"                 { return Parser::token::COLON; }
"->"                { return Parser::token::RARROW; }
","                 { return Parser::token::COMMA; }
<<EOF>>             { return Parser::token::YYEOF; }
.                   { yylval->emplace<std::string>(YYText()); return Parser::token::ERROR; }

%%

int yyFlexLexer::yylex() {
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}
