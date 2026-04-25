%{
#include "parser.tab.h"
#include <string.h>          /* for strdup */
void yyerror(const char *);  /* forward declaration */
%}

DIGIT   [0-9]
ID      [a-zA-Z_][a-zA-Z0-9_]*

%%
"int"           { return INT; }
"float"         { return FLOAT; }
"do"            { return DO; }
"while"         { return WHILE; }
"printf"        { return PRINTF; }

{ID}            { yylval.str = strdup(yytext); return ID; }

{DIGIT}+"."{DIGIT}+  { yylval.fval = atof(yytext); return FLOAT_NUM; }
{DIGIT}+             { yylval.ival = atoi(yytext); return INT_NUM; }

\"%d\"          { yylval.str = strdup(yytext); return STRING; }
\"%f\"          { yylval.str = strdup(yytext); return STRING; }

"+"             { return '+'; }
"<"             { return '<'; }
"="             { return '='; }
";"             { return ';'; }
","             { return ','; }
"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }

[ \t\n]         /* skip whitespace */
.               { yyerror("Illegal character"); }
%%

int yywrap() { return 1; }