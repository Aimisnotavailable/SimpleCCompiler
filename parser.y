%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

/* Simple symbol table for ints */
typedef struct {
    char *name;
    int value;
} Var;

#define MAX_VARS 128
static Var vars[MAX_VARS];
static int var_count = 0;

static int find_var(const char *name) {
    for (int i = 0; i < var_count; ++i) {
        if (strcmp(vars[i].name, name) == 0) return i;
    }
    return -1;
}

static void set_var(const char *name, int value) {
    int idx = find_var(name);
    if (idx >= 0) {
        vars[idx].value = value;
    } else {
        if (var_count < MAX_VARS) {
            vars[var_count].name = strdup(name);
            vars[var_count].value = value;
            ++var_count;
        } else {
            fprintf(stderr, "Symbol table full\n");
        }
    }
}

static int get_var_value(const char *name, int *out) {
    int idx = find_var(name);
    if (idx >= 0) {
        *out = vars[idx].value;
        return 1;
    }
    return 0;
}

static void print_vars(void) {
    for (int i = 0; i < var_count; ++i) {
        printf("%s = %d\n", vars[i].name, vars[i].value);
    }
}

%}

%union {
    int num;
    char *id;
}

/* token declarations */
%token <num> NUMBER
%token <id> ID
%token INT

/* declare nonterminal types */
%type <num> expr

%start program

%%

program:
    /* empty */
  | program stmt
  ;

stmt:
    decl_stmt
  | assign_stmt
  ;

decl_stmt:
    INT ID '=' expr ';' {
        set_var($2, $4);
        free($2);
    }
  ;

assign_stmt:
    ID '=' expr ';' {
        set_var($1, $3);
        free($1);
    }
  ;

expr:
    NUMBER { $$ = $1; }
  | ID {
        int val;
        if (get_var_value($1, &val)) {
            $$ = val;
        } else {
            fprintf(stderr, "Undefined variable %s, using 0\n", $1);
            $$ = 0;
        }
        free($1);
    }
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}

int main(void) {
    if (yyparse() == 0) {
        print_vars();
        return 0;
    } else {
        return 1;
    }
}
