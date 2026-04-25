flex lexer.l
bison -d parser.y
gcc -o compiler.exe emu8086.c parser.tab.c lex.yy.c