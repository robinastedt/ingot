%{
#include <parser/Parser.hh>
#include <parser/Scanner.hh>

#include <cassert>

#define YY_DECL int ingot::parser::Scanner::lex(ingot::parser::Parser::semantic_type *yylval)
#define YY_USER_ACTION setColumn(getColumn() + YYLeng());

static std::string stringFromToken(const std::string& str);
%}

%option c++ interactive noyywrap yylineno nodefault outfile="Scanner.cc"

integer         ([[:digit:]]+)

ident           ([a-zA-Z]+[a-zA-Z0-9]*)

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
"i8"                { return Parser::token::TYPE_INT8; }
"i64"               { return Parser::token::TYPE_INT64; }
{integer}           { yylval->emplace<int64_t>(strtoll(YYText(), nullptr, 10)); return Parser::token::INTEGER; }
{ident}             { yylval->emplace<std::string>(YYText()); return Parser::token::IDENT; }
{string}            { yylval->emplace<std::string>(stringFromToken(YYText())); return Parser::token::STRING; }
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
"["                 { return Parser::token::LBRACKET; }
"]"                 { return Parser::token::RBRACKET; }
<<EOF>>             { return Parser::token::YYEOF; }
.                   { yylval->emplace<std::string>(YYText()); return Parser::token::ERROR; }

%%

int yyFlexLexer::yylex() {
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}

static std::string stringFromToken(const std::string& str) {
    assert(str.size() >= 2);
    std::string result;
    bool escape = false;
    for (char c : std::string_view(str).substr(1, str.size() - 2)) {
        if (c == '\\') {
            escape = true;
        } else if (escape) {
            escape = false;
            switch (c) {
                case 'a':   result += '\a'; break; // 07	Alert (Beep, Bell) (added in C89)[1]
                case 'b':   result += '\b'; break; //	08	Backspace
                case 'f':   result += '\f'; break; //	0C	Formfeed Page Break
                case 'n':   result += '\n'; break; //	0A	Newline (Line Feed); see notes below
                case 'r':   result += '\r'; break; //	0D	Carriage Return
                case 't':   result += '\t'; break; //	09	Horizontal Tab
                case 'v':   result += '\v'; break; //	0B	Vertical Tab
                case '\\':  result += '\\'; break; //	5C	Backslash
                case '\'':  result += '\''; break; //	27	Apostrophe or single quotation mark
                case '\"':  result += '\"'; break; //	22	Double quotation mark
                default: break;; // ignore
            }
        } else {
            result += c;
        }
    }
    return result;
}