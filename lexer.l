%{
#include "parser.tab.h"
#include <string.h>
%}

%%

[ \t\r\n]+               ; /* skip whitespace */
"int"                    { return INT; }
[0-9]+                   {
                            yylval.num = atoi(yytext);
                            return NUMBER;
                         }
[a-zA-Z_][a-zA-Z0-9_]*    {
                            yylval.id = strdup(yytext);
                            return ID;
                         }
"="                      { return '='; }
";"                      { return ';'; }
.                        { /* ignore unknown chars */ }

%%
int yywrap(void) { return 1; }
