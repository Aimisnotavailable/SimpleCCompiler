%defines                     /* ← generate parser.tab.h */

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>

    /* DataType is only needed inside the parser actions, not in the header. */
    typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_ERROR, TYPE_VOID } DataType;

    /* ---------- Simple symbol table ---------- */
    typedef struct Sym {
        char* name;
        DataType type;
        struct Sym* next;
    } Symbol;
    Symbol* sym_table = NULL;

    Symbol* lookup(const char* name) {
        Symbol* s = sym_table;
        while (s) { if (!strcmp(s->name, name)) return s; s = s->next; }
        return NULL;
    }
    void install(const char* name, DataType t) {
        Symbol* s = malloc(sizeof(Symbol));
        s->name = strdup(name);
        s->type = t;
        s->next = sym_table;
        sym_table = s;
    }

    /* ---------- Error collection ---------- */
    #define MAX_ERR 100
    char* errors[MAX_ERR];
    int err_cnt = 0;

    void add_error(const char* fmt, ...) {
        char buf[256];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        if (err_cnt < MAX_ERR) errors[err_cnt++] = strdup(buf);
    }

    void print_errors() {
        if (err_cnt == 0)
            printf("Code is valid.\n");
        else {
            printf("Errors (%d):\n", err_cnt);
            for (int i = 0; i < err_cnt; i++)
                printf("  - %s\n", errors[i]);
        }
    }

    /* ---------- printf argument tracking ---------- */
    DataType arg_types[20];
    int arg_count = 0;
    extern char* current_format_string;   /* set by lexer */

    void reset_args() { arg_count = 0; current_format_string = NULL; }
    void add_arg(DataType t) { if (arg_count < 20) arg_types[arg_count++] = t; }

    void check_printf() {
        if (!current_format_string) {
            add_error("Semantic error: printf first argument must be a string literal");
            return;
        }
        char* p = current_format_string + 1;   /* skip opening " */
        int expected[20], exp_cnt = 0;
        while (*p && *p != '"') {
            if (*p == '%') {
                p++;
                if (*p == 'd') expected[exp_cnt++] = TYPE_INT;
                else if (*p == 'f') expected[exp_cnt++] = TYPE_FLOAT;
                else if (*p == 's') expected[exp_cnt++] = TYPE_STRING;
                else { add_error("Semantic error: unknown format specifier '%%%c'", *p); return; }
            }
            p++;
        }
        if (exp_cnt != arg_count) {
            add_error("Semantic error: printf expects %d data arguments, got %d", exp_cnt, arg_count);
            return;
        }
        for (int i = 0; i < exp_cnt; i++) {
            if (expected[i] == TYPE_INT && arg_types[i] != TYPE_INT)
                add_error("Semantic error: argument %d to printf should be int, but got float", i+1);
            else if (expected[i] == TYPE_FLOAT && arg_types[i] != TYPE_FLOAT)
                add_error("Semantic error: argument %d to printf should be float, but got int", i+1);
        }
    }

    /* ---------- Syntax error handling ---------- */
    extern int yylineno;
    void yyerror(const char* s) {
        add_error("Syntax error at line %d: %s", yylineno, s);
    }
%}

/* ---------- Bison union (now uses plain int for the semantic type) ---------- */
%union {
    int   intVal;
    float floatVal;
    char* str;
    int   type;          /* was DataType; now a plain int */
}

/* ---------- Tokens ---------- */
%token INT_KEYWORD FLOAT_KEYWORD DO WHILE
%token <str>    ID STRING_LITERAL
%token <intVal> INT_CONST
%token <floatVal> FLOAT_CONST

/* ---------- Precedence ---------- */
%left '='
%left '<' '>'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

/* ---------- Types of non‑terminals (now refer to plain int) ---------- */
%type <type> expression
%type <type> function_call
%type <type> argument

%start program

%%
program:
    block_items
;

block_items:
    block_item
  | block_items block_item
;

block_item:
    declaration ';'
  | statement
  | error ';'   { yyerrok; }
;

declaration:
    INT_KEYWORD ID                {
                                      if (!lookup($2)) install($2, TYPE_INT);
                                      else add_error("Semantic error: redeclaration of '%s'", $2);
                                  }
  | INT_KEYWORD ID '=' expression {
                                      if (!lookup($2)) install($2, TYPE_INT);
                                      else add_error("Semantic error: redeclaration of '%s'", $2);
                                      if ($4 != TYPE_INT)
                                          add_error("Semantic error: type mismatch in init of '%s'", $2);
                                  }
  | FLOAT_KEYWORD ID              {
                                      if (!lookup($2)) install($2, TYPE_FLOAT);
                                      else add_error("Semantic error: redeclaration of '%s'", $2);
                                  }
  | FLOAT_KEYWORD ID '=' expression {
                                      if (!lookup($2)) install($2, TYPE_FLOAT);
                                      else add_error("Semantic error: redeclaration of '%s'", $2);
                                      if ($4 != TYPE_FLOAT && $4 != TYPE_INT)
                                          add_error("Semantic error: type mismatch in init of '%s'", $2);
                                  }
;

statement:
    compound_stmt
  | do_while_stmt
  | expression_stmt
;

expression_stmt:
    expression ';'
  | ';'
;

compound_stmt:
    '{' block_items '}'
;

do_while_stmt:
    DO statement WHILE '(' expression ')' ';' {
        if ($5 != TYPE_INT && $5 != TYPE_FLOAT)
            add_error("Semantic error: do-while condition must be numeric");
    }
;

expression:
    ID '=' expression      {
                               Symbol* s = lookup($1);
                               if (!s) add_error("Semantic error: '%s' undeclared", $1);
                               else {
                                   if (s->type == TYPE_INT && $3 != TYPE_INT)
                                       add_error("Semantic error: assigning float to int variable '%s'", $1);
                                   else if (s->type == TYPE_FLOAT && $3 != TYPE_FLOAT && $3 != TYPE_INT)
                                       add_error("Semantic error: invalid assignment to float '%s'", $1);
                                   $$ = s->type;
                               }
                           }
  | expression '+' expression   { $$ = ($1 == TYPE_INT && $3 == TYPE_INT) ? TYPE_INT : TYPE_FLOAT; }
  | expression '-' expression   { $$ = ($1 == TYPE_INT && $3 == TYPE_INT) ? TYPE_INT : TYPE_FLOAT; }
  | expression '*' expression   { $$ = ($1 == TYPE_INT && $3 == TYPE_INT) ? TYPE_INT : TYPE_FLOAT; }
  | expression '/' expression   { $$ = ($1 == TYPE_INT && $3 == TYPE_INT) ? TYPE_INT : TYPE_FLOAT; }
  | expression '<' expression   { $$ = TYPE_INT; }
  | expression '>' expression   { $$ = TYPE_INT; }
  | '-' expression %prec UMINUS { $$ = $2; }
  | '(' expression ')'          { $$ = $2; }
  | INT_CONST                   { $$ = TYPE_INT; }
  | FLOAT_CONST                 { $$ = TYPE_FLOAT; }
  | STRING_LITERAL              { $$ = TYPE_STRING; }
  | ID                          {
                                    Symbol* s = lookup($1);
                                    if (!s) add_error("Semantic error: '%s' undeclared", $1);
                                    else $$ = s->type;
                                }
  | function_call               { $$ = $1; }
;

function_call:
    ID '(' ')'                  {
                                    if (!strcmp($1, "printf"))
                                        add_error("Semantic error: printf requires a format string");
                                    $$ = TYPE_VOID;
                                }
  | ID '(' argument_list ')'   {
                                    $$ = TYPE_VOID;
                                    if (!strcmp($1, "printf")) {
                                        check_printf();
                                    }
                                    reset_args();
                                }
;

argument_list:
    argument
  | argument_list ',' argument
;

argument:
    expression                   {
                                    add_arg($1);
                                    $$ = $1;
                                }
;

%%

int main(int argc, char** argv) {
    extern FILE* yyin;
    if (argc > 1) yyin = fopen(argv[1], "r");
    yyparse();
    print_errors();
    return 0;
}