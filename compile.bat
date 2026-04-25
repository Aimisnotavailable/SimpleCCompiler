flex lexer.l
bison -d parser.y
gcc -o compiler.exe parser.tab.c lex.yy.c emu8086.c