%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);
void yyerror(const char *s);
extern int yylineno;

/* ---- type enumeration ---- */
typedef enum { TYPE_INT, TYPE_FLOAT } DataType;

/* ---- error recording ---- */
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

/* ---- symbol table (global linked list) ---- */
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

/* lookup: finds most recent declaration in current or enclosing scopes */
SymEntry* lookup(const char *name) {
    SymEntry *p;
    for (p = sym_table; p; p = p->next)
        if (strcmp(p->name, name) == 0 && p->scope <= current_scope)
            return p;
    return NULL;
}

/* check for redeclaration in current scope */
int redeclared(const char *name) {
    SymEntry *p;
    for (p = sym_table; p; p = p->next){
        if (strcmp(p->name, name) == 0 && p->scope <= current_scope)
            return 1;
    }
    return 0;
}

/* printing results */
void print_symbol_table() {
    printf("\n=== Symbol Table ===\n");
    SymEntry *p;
    for (p = sym_table; p; p = p->next)
        printf("Name: %s  Type: %s  Line: %d  Scope: %d\n",
               p->name, p->type == TYPE_INT ? "int" : "float",
               p->line, p->scope);
}

void print_errors() {
    printf("\n=== Errors/Warnings ===\n");
    if (!errors) { printf("None.\n"); return; }
    ErrorMsg *e;
    for (e = errors; e; e = e->next)
        printf("Line %d: %s\n", e->line, e->msg);
}
%}

%union {
    int ival;          /* for int constants */
    float fval;        /* for float constants */
    char *sval;        /* for identifiers and string literals */
    int type;          /* expression type: TYPE_INT or TYPE_FLOAT */
}

%token T_INT T_FLOAT T_DO T_WHILE T_PRINTF
%token <ival> INT_CONST
%token <fval> FLOAT_CONST
%token <sval> ID STRING_LITERAL

%type <type> expr term

%locations

%%
program: stmt_list                { /* done */ }
       ;

stmt_list: stmt
         | stmt_list stmt
         ;

stmt: decl ';'
    | assign ';'
    | printf_stmt ';'
    | do_while_stmt
    | block
    ;

decl: T_INT ID '=' expr {
         if (redeclared($2))
             add_error(yylineno, "redeclaration of variable");
         else {
             add_sym($2, TYPE_INT, yylineno);
             if ($4 != TYPE_INT)
                 add_error(yylineno,
                           "initializer type does not match declared type (int)");
         }
         free($2);
     }
    | T_FLOAT ID '=' expr {
         if (redeclared($2))
             add_error(yylineno, "redeclaration of variable");
         else {
             add_sym($2, TYPE_FLOAT, yylineno);
             if ($4 != TYPE_FLOAT)
                 add_error(yylineno,
                           "initializer type does not match declared type (float)");
         }
         free($2);
     }
    ;

assign: ID '=' expr {
         SymEntry *s = lookup($1);
         if (!s){
             add_error(yylineno, "assignment to undeclared variable");
         }
         else if (s->type != $3)
             add_error(yylineno, "assignment type mismatch");
         free($1);
       }
     ;

printf_stmt: T_PRINTF '(' STRING_LITERAL ',' expr ')' {
         char *fmt = $3;
         if (strcmp(fmt, "%d") == 0 && $5 != TYPE_INT)
             add_error(yylineno, "'%%d' expects int but argument is float");
         
         else if (strcmp(fmt, "%f") == 0 && $5 != TYPE_FLOAT)
             add_error(yylineno, "'%%f' expects float but argument is int");
         free($3);
     }
     ;

do_while_stmt: T_DO block T_WHILE '(' expr ')' ';' {
         if ($5 == -1) /* dummy error type not used */;
         /* any scalar allowed; nothing to enforce */
     }
     ;

block: '{'  { current_scope++; }
        stmt_list '}' { current_scope--; }
     ;

expr: expr '+' term {
         $$ = ($1 == TYPE_FLOAT || $3 == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
     }
    | expr '<' term {

    }
    | term  { $$ = $1; }
    ;

term: ID {
         SymEntry *s = lookup($1);

         if (!s) {
             add_error(yylineno, "undeclared identifier");
             $$ = TYPE_INT;   /* dummy type to avoid cascading */
         } else
             $$ = s->type;
         free($1);
     }
    | INT_CONST   { $$ = TYPE_INT; }
    | FLOAT_CONST { $$ = TYPE_FLOAT; }
    | '(' expr ')' { $$ = $2; }
    ;
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