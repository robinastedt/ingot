%{
#include <ingot/Error.hh>
#include <ingot/parser/Parser.hh>
#include <ingot/parser/Scanner.hh>

#include <cassert>

#define YY_DECL int ingot::parser::Scanner::lex(ingot::parser::Parser::semantic_type *yylval, ingot::parser::location* yylloc)
#define YY_USER_ACTION \
    yylloc->begin.line = yylloc->end.line = lineno(); \
    yylloc->begin.column = yylloc->end.column; \
    yylloc->end.column += YYLeng(); \
    

static std::string stringFromToken(std::string_view str);
static char charFromToken(std::string_view str);
%}

%option c++ interactive noyywrap yylineno nodefault outfile="Scanner.cc"

integer         ([[:digit:]]+)
typeint         (i{integer})
char            ('(\\.|[^'\\])')

ident           ([a-zA-Z]+[a-zA-Z0-9]*)

whitespace      ([ \t]+)
linebreak       (\n+)
string         (\"(\\.|[^"\\])*\")

%x COMMENT

%%

"#"                 { BEGIN(COMMENT); }
<COMMENT>"\n"       { BEGIN(INITIAL); yylloc->begin.column = yylloc->end.column = 1; }
<COMMENT><<EOF>>    { BEGIN(INITIAL); return Parser::token::YYEOF; }
<COMMENT>.          { }
{whitespace}        { }
{linebreak}         { yylloc->begin.column = yylloc->end.column = 1; }
{char}              { yylval->emplace<int64_t>(charFromToken({YYText(), (size_t)YYLeng()})); return Parser::token::INTEGER; }
{typeint}           { yylval->emplace<int64_t>(strtoll(YYText() + 1, nullptr, 10)); return Parser::token::TYPE_INT; }
{integer}           { yylval->emplace<int64_t>(strtoll(YYText(), nullptr, 10)); return Parser::token::INTEGER; }
"if"                { return Parser::token::KW_IF; }
"then"                { return Parser::token::KW_THEN; }
"else"                { return Parser::token::KW_ELSE; }
{ident}             { yylval->emplace<std::string>(YYText()); return Parser::token::IDENT; }
{string}            { yylval->emplace<std::string>(stringFromToken({YYText(), (size_t)YYLeng()})); return Parser::token::STRING; }
"+"                 { return Parser::token::PLUS; }
"-"                 { return Parser::token::MINUS; }
"*"                 { return Parser::token::MULTIPLY; }
"/"                 { return Parser::token::DIVIDE; }
"%"                 { return Parser::token::MODULO; }
"=="                { return Parser::token::EQUALITY; }
"!="                { return Parser::token::INEQUALITY; }
"("                 { return Parser::token::LPAREN; }
")"                 { return Parser::token::RPAREN; }
"="                 { return Parser::token::ASSIGN; }
":"                 { return Parser::token::COLON; }
"->"                { return Parser::token::RARROW; }
","                 { return Parser::token::COMMA; }
"["                 { return Parser::token::LBRACKET; }
"]"                 { return Parser::token::RBRACKET; }
<<EOF>>             { return Parser::token::YYEOF; }
.                   { yylval->emplace<std::string>(YYText()); return Parser::token::ERROR; }

%%

int yyFlexLexer::yylex() {
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}

static char charFromToken(std::string_view str) {
    const std::string res = stringFromToken(str);
    if (res.size() != 1) {
        throw internal_error("Token is not a single character");
    }
    return res[0];
}

static std::string stringFromToken(std::string_view str) {
    assert(str.size() >= 2);
    std::string result;
    bool escape = false;
    for (char c : str.substr(1, str.size() - 2)) {
        if (c == '\\') {
            escape = true;
        } else if (escape) {
            escape = false;
            switch (c) {
                case 'a':   result += '\a'; break; //   07 Alert (Beep, Bell) (added in C89)[1]
                case 'b':   result += '\b'; break; //   08 Backspace
                case 'f':   result += '\f'; break; //   0C Formfeed Page Break
                case 'n':   result += '\n'; break; //   0A Newline (Line Feed); see notes below
                case 'r':   result += '\r'; break; //   0D Carriage Return
                case 't':   result += '\t'; break; //   09 Horizontal Tab
                case 'v':   result += '\v'; break; //   0B Vertical Tab
                case '\\':  result += '\\'; break; //   5C Backslash
                case '\'':  result += '\''; break; //   27 Apostrophe or single quotation mark
                case '\"':  result += '\"'; break; //   22 Double quotation mark
                default: break;; // ignore
            }
        } else {
            result += c;
        }
    }
    return result;
}