%{
// -- DECLARATIONS

#include "output.hpp"
#include "nodes.hpp"
#include "parser.tab.h"

%}

%option yylineno
%option noyywrap

digit   ([0-9])
letter  ([a-zA-Z])
whitespace  ([\t\n\r ])

%%

void                            {return VOID;}
\(                              {return LPAREN;}
\-                              {return MINUS;}
\)                              {return RPAREN;}
int                             {return INT;}
byte                            {return BYTE;}
bool                            {return BOOL;}
and                             {return AND;}
not                             {return NOT;}
true                            {return TRUE;}
return                          {return RETURN;}
\>                              {return GT;}
if                              {return IF;}
continue                        {return CONTINUE;}
;                               {return SC;}
,                               {return COMMA;}
\{                              {return LBRACE;}
\/                              {return DIV;}
\}                              {return RBRACE;}
\<=                             {return LTE;}
\]                              {return RBRACK;}
while                           {return WHILE;}
!=                              {return NEQ;}
break                           {return BREAK;}
=                               {return ASSIGN;}
==                              {return EQ;}
false                           {return FALSE;}
\[                              {return LBRACK;}
\<                              {return LT;}
or                              {return OR;}
\>=                             {return GTE;}
\+                              {return PLUS;}
else                            {return ELSE;}
\*                              {return MULT;}
[a-zA-Z]([a-zA-Z0-9])*          {yylval = std::make_shared<ast::ID>(yytext); return ID;}
[1-9]{digit}*|0                 {yylval = std::make_shared<ast::Num>(yytext); return NUM;}
([1-9]{digit}*|0)b              {yylval = std::make_shared<ast::NumB>(yytext); return NUM_B;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"   {yylval = std::make_shared<ast::String>(yytext); return STRING;}
\/\/[^\r\n]*[\r|\n|\r\n]?       {;}
{whitespace}                    {;}
.                               {output::errorLex(yylineno);}
%%
