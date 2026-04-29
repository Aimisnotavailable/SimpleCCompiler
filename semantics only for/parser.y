%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);
void yyerror(const char *s);
extern int yylineno;

#define RED    "\033[1;31m"
#define GREEN  "\033[1;32m"
#define CYAN   "\033[1;36m"
#define YELLOW "\033[1;33m"
#define RESET  "\033[0m"

typedef enum { TYPE_INT, TYPE_FLOAT } DataType;

/* error linked list */
typedef struct error_msg {
    int line;
    char *msg;
    struct error_msg *next;
} ErrorMsg;
ErrorMsg *errors = NULL;

void add_error(int line, const char *msg) {
    ErrorMsg *e = malloc(sizeof(ErrorMsg));
    e->line = line;
    e->msg = strdup(msg);
    e->next = errors;
    errors = e;
}

/* symbol table */
typedef struct sym_entry {
    char *name;
    DataType type;
    int scope;
    int line;
    struct sym_entry *next;
} SymEntry;
SymEntry *sym_table = NULL;
int current_scope = 0;

void add_sym(const char *name, DataType type, int line) {
    SymEntry *s = malloc(sizeof(SymEntry));
    s->name = strdup(name);
    s->type = type;
    s->scope = current_scope;
    s->line = line;
    s->next = sym_table;
    sym_table = s;
}

SymEntry* lookup(const char *name) {
    for (SymEntry *p = sym_table; p; p = p->next)
        if (strcmp(p->name, name) == 0 && p->scope <= current_scope)
            return p;
    return NULL;
}

int redeclared(const char *name) {
    for (SymEntry *p = sym_table; p; p = p->next)
        if (strcmp(p->name, name) == 0 && p->scope <= current_scope)
            return 1;
    return 0;
}

/* ---- improved printing ---- */
static int cmp_sym(const void *a, const void *b) {
    SymEntry *sa = *(SymEntry**)a, *sb = *(SymEntry**)b;
    if (sa->scope != sb->scope) return sa->scope - sb->scope;
    if (sa->line != sb->line) return sa->line - sb->line;
    return strcmp(sa->name, sb->name);
}

/* ---- ASCII-friendly symbol table printer ---- */
void print_symbol_table() {
    int n = 0;
    for (SymEntry *p = sym_table; p; p = p->next) n++;
    SymEntry **arr = malloc(n * sizeof(SymEntry*));
    SymEntry *p = sym_table;
    for (int i = 0; i < n; i++) { arr[i] = p; p = p->next; }
    qsort(arr, n, sizeof(SymEntry*), cmp_sym);

    printf("\n" CYAN "+------------------------------------+\n");
    printf(      "|        SYMBOL TABLE                |\n");
    printf(      "+------------------------------------+\n");
    printf(      "| %-12s | %-6s | %-4s | %-5s |\n", "Name","Type","Line","Scope");
    printf(      "+------------------------------------+\n");
    for (int i = 0; i < n; i++)
        printf("| %-12s | %-6s | %4d | %5d |\n",
               arr[i]->name,
               arr[i]->type == TYPE_INT ? "int" : "float",
               arr[i]->line, arr[i]->scope);
    printf("+------------------------------------+\n" RESET);
    free(arr);
}

/* ---- ASCII-friendly error printer ---- */
void print_errors() {
    printf("\n" RED "+----------- Error Summary ------------+\n" RESET);
    if (!errors) {
        printf(GREEN "| None.                                 |\n" RESET);
    } else {
        for (ErrorMsg *e = errors; e; e = e->next)
            printf(RED "| Line %-4d: %-27s |\n" RESET, e->line, e->msg);
    }
    printf(RED "+---------------------------------------+\n" RESET);
}
%}

%union {
    int ival;
    float fval;
    char *sval;
    int type;
}

%token T_INT T_FLOAT T_FOR T_PRINTF
%token <ival> INT_CONST
%token <fval> FLOAT_CONST
%token <sval> ID STRING_LITERAL

%type <type> expr term

%locations

%%
program: stmt_list ;

stmt_list: stmt
         | stmt_list stmt ;

stmt: decl ';'
    | assign ';'
    | printf_stmt ';'
    | for_stmt
    | block ;

decl: T_INT ID '=' expr {
        if (redeclared($2)) add_error(yylineno, "redeclaration of variable");
        else {
            add_sym($2, TYPE_INT, yylineno);
            if ($4 != TYPE_INT) add_error(yylineno,
                "initializer type does not match declared type (int)");
        }
        free($2);
     }
    | T_FLOAT ID '=' expr {
        if (redeclared($2)) add_error(yylineno, "redeclaration of variable");
        else {
            add_sym($2, TYPE_FLOAT, yylineno);
            if ($4 != TYPE_FLOAT) add_error(yylineno,
                "initializer type does not match declared type (float)");
        }
        free($2);
     } ;

assign: ID '=' expr {
        SymEntry *s = lookup($1);
        if (!s) add_error(yylineno, "assignment to undeclared variable");
        else if (s->type != $3) add_error(yylineno, "assignment type mismatch");
        free($1);
       } ;

printf_stmt: T_PRINTF '(' STRING_LITERAL ',' expr ')' {
        if (strcmp($3, "%d") == 0 && $5 != TYPE_INT)
            add_error(yylineno, "'%%d' expects int but argument is float");
        else if (strcmp($3, "%f") == 0 && $5 != TYPE_FLOAT)
            add_error(yylineno, "'%%f' expects float but argument is int");
        free($3);
     } ;

/* ---- for loop ---- */
for_stmt: T_FOR { current_scope++; }
          '(' for_init ';' expr ';' assign_part ')' stmt
          { current_scope--; } ;

for_init: decl_part
        | assign_part ;

decl_part: T_INT ID '=' expr {
            if (redeclared($2)) add_error(yylineno, "redeclaration of variable");
            else {
                add_sym($2, TYPE_INT, yylineno);
                if ($4 != TYPE_INT) add_error(yylineno,
                    "initializer type does not match declared type (int)");
            }
            free($2);
         }
        | T_FLOAT ID '=' expr {
            if (redeclared($2)) add_error(yylineno, "redeclaration of variable");
            else {
                add_sym($2, TYPE_FLOAT, yylineno);
                if ($4 != TYPE_FLOAT) add_error(yylineno,
                    "initializer type does not match declared type (float)");
            }
            free($2);
         } ;

assign_part: ID '=' expr {
            SymEntry *s = lookup($1);
            if (!s) add_error(yylineno, "assignment to undeclared variable");
            else if (s->type != $3) add_error(yylineno, "assignment type mismatch");
            free($1);
            } ;

block: '{' { current_scope++; } stmt_list '}' { current_scope--; } ;

expr: expr '+' term {
        $$ = ($1 == TYPE_FLOAT || $3 == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
     }
    | expr '<' term { $$ = TYPE_INT; }
    | term { $$ = $1; } ;

term: ID {
        SymEntry *s = lookup($1);
        if (!s) {
            add_error(yylineno, "undeclared identifier");
            $$ = TYPE_INT;
        } else $$ = s->type;
        free($1);
     }
    | INT_CONST   { $$ = TYPE_INT; }
    | FLOAT_CONST { $$ = TYPE_FLOAT; }
    | '(' expr ')' { $$ = $2; } ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
}

int main() {
    yyparse();
    print_symbol_table();
    print_errors();
    return 0;
}