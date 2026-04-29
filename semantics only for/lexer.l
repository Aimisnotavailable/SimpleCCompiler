%{
#include <string.h>
#include <stdio.h>
#include "parser.tab.h"
%}

%option yylineno
%option noyywrap

DIGIT      [0-9]
INT_CONST  {DIGIT}+
FLOAT_CONST {DIGIT}+"."{DIGIT}*
ID         [a-zA-Z_][a-zA-Z0-9_]*
STRING_LIT \"[^"]*\"

%%
"int"      { return T_INT; }
"float"    { return T_FLOAT; }
"for"      { return T_FOR; }
"printf"   { return T_PRINTF; }

"+"        { return '+'; }
"="        { return '='; }
";"        { return ';'; }
"("        { return '('; }
")"        { return ')'; }
"{"        { return '{'; }
"}"        { return '}'; }
"<"        { return '<'; }

{INT_CONST}    { yylval.ival = atoi(yytext); return INT_CONST; }
{FLOAT_CONST}  { yylval.fval = atof(yytext); return FLOAT_CONST; }
{ID}           { yylval.sval = strdup(yytext); return ID; }
{STRING_LIT}   { yytext[strlen(yytext)-1] = '\0';
                 yylval.sval = strdup(yytext+1); return STRING_LITERAL; }
[ \t\r]        /* skip */
\n             
"//".*         /* skip comments */
.              { return yytext[0]; }
%%