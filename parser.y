%{
/* ============================================================
   parser.y – Complete compiler front‑end with TAC generation
   ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef VARTYPE_DEFINED
#define VARTYPE_DEFINED
typedef enum { VAR_INT, VAR_FLOAT } VarType;
#endif

/* ------------------------------------------------------------------
   Forward declarations for the backend (code generator)
   ------------------------------------------------------------------ */
void generate_emu8086(const char *filename);


/* ------------------------------------------------------------------
   AST Node definitions
   ------------------------------------------------------------------ */
typedef enum {
    NODE_INT, NODE_FLOAT, NODE_ID,
    NODE_ADD, NODE_LT,
    NODE_ASSIGN,
    NODE_PRINTF,
    NODE_BLOCK,
    NODE_LOOP
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    union {
        int    ival;                    /* NODE_INT */
        float  fval;                    /* NODE_FLOAT */
        char  *sval;                    /* NODE_ID */
        struct {
            struct ASTNode *left, *right;
        } bin;                          /* NODE_ADD, NODE_LT */
        struct {
            char *name;
            struct ASTNode *expr;
        } assign;                       /* NODE_ASSIGN */
        struct {
            char *fmt;
            struct ASTNode *expr;
        } print;                        /* NODE_PRINTF */
        struct {
            struct ASTNode *body, *cond;
        } loop;                         /* NODE_LOOP */
        struct {
            struct ASTNode **stmts;
            int cnt;
        } block;                        /* NODE_BLOCK */
    } d;
} ASTNode;

void generate_ast_dot(ASTNode *root, const char *filename);

/* Constructor prototypes */
ASTNode* make_int(int val);
ASTNode* make_float(float val);
ASTNode* make_id(char *name);
ASTNode* make_add(ASTNode *l, ASTNode *r);
ASTNode* make_lt(ASTNode *l, ASTNode *r);
ASTNode* make_assign(char *name, ASTNode *expr);
ASTNode* make_printf(char *fmt, ASTNode *expr);
ASTNode* make_block(ASTNode **stmts, int cnt);
ASTNode* make_loop(ASTNode *body, ASTNode *cond);
void append_stmt(ASTNode *block, ASTNode *stmt);

/* ------------------------------------------------------------------
   Symbol table (global scope only)
   ------------------------------------------------------------------ */
typedef struct {
    char *name;
    VarType type;
    ASTNode *init;   /* initialiser expression, NULL if none */
    int is_temp;     /* 1 if temporary variable */
} Symbol;

#define MAX_SYMBOLS 200
Symbol symtab[MAX_SYMBOLS];
int sym_cnt = 0;

int  sym_insert(char *name, VarType type, ASTNode *init, int is_temp);
Symbol* sym_lookup(char *name);
VarType sym_get_type(char *name);

/* ------------------------------------------------------------------
   Type inference for expressions
   ------------------------------------------------------------------ */
VarType node_type(ASTNode *n);

/* ------------------------------------------------------------------
   Three‑address code (TAC) structures and globals
   ------------------------------------------------------------------ */
typedef enum {
    OP_ADD, OP_LT, OP_ASSIGN,
    OP_PRINT_INT, OP_PRINT_FLOAT,
    OP_LABEL, OP_JMP, OP_JMPTRUE, OP_HALT
} OpCode;

typedef struct {
    OpCode op;
    char *dest;
    char *src1;
    char *src2;
    VarType dest_type;      /* type of result (for ADD) */
} Quad;

#define MAX_QUADS 500
Quad quads[MAX_QUADS];
int nquad = 0;

/* For constants and labels */
typedef struct {
    char *label;
    VarType type;
    union {
        int ival;
        float fval;
    } val;
} Constant;

#define MAX_CONST 100
Constant const_pool[MAX_CONST];
int nconst = 0;

int temp_counter = 0;
int label_counter = 0;

/* TAC helpers */
void emit_quad(OpCode op, char *dest, char *src1, char *src2, VarType dest_type);
char* new_temp(VarType type);
char* new_label(void);
char* new_int_const(int val);
char* new_float_const(float val);

/* Forward declarations for TAC generation */
char* gen_expr(ASTNode *node);
void  gen_stmt(ASTNode *stmt);
void  gen_program(ASTNode *root);

/* ------------------------------------------------------------------
   Root of the AST, set during parsing
   ------------------------------------------------------------------ */
ASTNode *ast_root = NULL;

/* ------------------------------------------------------------------
   Error counter
   ------------------------------------------------------------------ */
int error_count = 0;

/* Override yyerror to count errors */
void yyerror(const char *msg) {
    fprintf(stderr, "Syntax error: %s\n", msg);
    error_count++;
}

/* ------------------------------------------------------------------
   Flex / Bison interface
   ------------------------------------------------------------------ */
extern int yylex();
extern int yyparse();
extern FILE *yyin;
%}

%code requires {
    #ifndef VARTYPE_DEFINED
    #define VARTYPE_DEFINED
    typedef enum { VAR_INT, VAR_FLOAT } VarType;
    #endif
}


/* ============================================================
   Bison declarations
   ============================================================ */
%union {
    int    ival;          /* INT_NUM */
    float  fval;          /* FLOAT_NUM */
    char  *str;           /* ID, STRING */
    struct ASTNode *node; /* expressions and statements */
    VarType dtype;        /* for Type nonterminal */
}

%token INT FLOAT DO WHILE PRINTF
%token<str> ID STRING
%token<ival> INT_NUM
%token<fval> FLOAT_NUM

%type<node> Program DeclList Declaration Stmt StmtList DoWhileStmt Expr Term PrintfStmt
%type<dtype> Type

%left '<'
%left '+'
%right '='

%start Program

%%
/* ============================================================
   Grammar rules with actions
   ============================================================ */
Program
    : DeclList DoWhileStmt   { ast_root = $2; }
    ;

DeclList
    : /* empty */    { $$ = NULL; }
    | DeclList Declaration
    ;

Declaration
    : Type ID '=' Expr ';'
      {
          if (!sym_insert($2, $1, $4, 0))
              fprintf(stderr, "Semantic error: redeclaration of '%s'\n", $2);
          /* Remember the expression for later initialisation code */
      }
    ;

Type
    : INT    { $$ = VAR_INT; }
    | FLOAT  { $$ = VAR_FLOAT; }
    ;

DoWhileStmt
    : DO '{' StmtList '}' WHILE '(' Expr ')' ';'
      { $$ = make_loop($3, $7); }
    ;

StmtList
    : /* empty */               { $$ = make_block(NULL, 0); }
    | StmtList Stmt             { append_stmt($1, $2); $$ = $1; }
    | StmtList error ';'        { yyerrok; }  /* error recovery */
    ;

Stmt
    : PrintfStmt
    | ID '=' Expr ';'
      {
          Symbol *sym = sym_lookup($1);
          if (!sym)
              fprintf(stderr, "Semantic error: undeclared variable '%s'\n", $1);
          else {
              VarType lhs = sym->type;
              VarType rhs = node_type($3);
              if (lhs != rhs)
                  fprintf(stderr, "Semantic error: type mismatch in assignment to '%s'\n", $1);
          }
          $$ = make_assign($1, $3);
      }
    ;

PrintfStmt
    : PRINTF '(' STRING ',' Expr ')' ';'
      {
          char *fmt = $3;
          ASTNode *expr = $5;
          VarType etype = node_type(expr);
          int ok = 0;
          if (strcmp(fmt, "\"%d\"") == 0 && etype == VAR_INT) ok = 1;
          else if (strcmp(fmt, "\"%f\"") == 0 && etype == VAR_FLOAT) ok = 1;
          if (!ok)
              fprintf(stderr, "Semantic error: format/type mismatch in printf\n");
          $$ = make_printf(fmt, expr);
      }
    ;

Expr
    : Expr '+' Expr   { $$ = make_add($1, $3); }
    | Expr '<' Expr   { $$ = make_lt($1, $3); }
    | Term
    ;

Term
    : ID               { $$ = make_id($1); }
    | INT_NUM          { $$ = make_int($1); }
    | FLOAT_NUM        { $$ = make_float($1); }
    | '(' Expr ')'     { $$ = $2; }
    ;

%%

/* ============================================================
   AST constructors
   ============================================================ */
ASTNode* make_node(NodeKind kind) {
    ASTNode *n = (ASTNode*)malloc(sizeof(ASTNode));
    n->kind = kind;
    return n;
}

ASTNode* make_int(int val) {
    ASTNode *n = make_node(NODE_INT);
    n->d.ival = val;
    return n;
}

ASTNode* make_float(float val) {
    ASTNode *n = make_node(NODE_FLOAT);
    n->d.fval = val;
    return n;
}

ASTNode* make_id(char *name) {
    ASTNode *n = make_node(NODE_ID);
    n->d.sval = strdup(name);
    return n;
}

ASTNode* make_add(ASTNode *l, ASTNode *r) {
    ASTNode *n = make_node(NODE_ADD);
    n->d.bin.left = l;
    n->d.bin.right = r;
    return n;
}

ASTNode* make_lt(ASTNode *l, ASTNode *r) {
    ASTNode *n = make_node(NODE_LT);
    n->d.bin.left = l;
    n->d.bin.right = r;
    return n;
}

ASTNode* make_assign(char *name, ASTNode *expr) {
    ASTNode *n = make_node(NODE_ASSIGN);
    n->d.assign.name = strdup(name);
    n->d.assign.expr = expr;
    return n;
}

ASTNode* make_printf(char *fmt, ASTNode *expr) {
    ASTNode *n = make_node(NODE_PRINTF);
    n->d.print.fmt = strdup(fmt);
    n->d.print.expr = expr;
    return n;
}

ASTNode* make_block(ASTNode **stmts, int cnt) {
    ASTNode *n = make_node(NODE_BLOCK);
    if (cnt > 0) {
        n->d.block.stmts = malloc(cnt * sizeof(ASTNode*));
        memcpy(n->d.block.stmts, stmts, cnt * sizeof(ASTNode*));
        n->d.block.cnt = cnt;
    } else {
        n->d.block.stmts = NULL;
        n->d.block.cnt = 0;
    }
    return n;
}

void append_stmt(ASTNode *block, ASTNode *stmt) {
    if (!block || block->kind != NODE_BLOCK) return;
    block->d.block.stmts = realloc(block->d.block.stmts,
                                   (block->d.block.cnt + 1) * sizeof(ASTNode*));
    block->d.block.stmts[block->d.block.cnt++] = stmt;
}

ASTNode* make_loop(ASTNode *body, ASTNode *cond) {
    ASTNode *n = make_node(NODE_LOOP);
    n->d.loop.body = body;
    n->d.loop.cond = cond;
    return n;
}

/* ============================================================
   Symbol table operations
   ============================================================ */
int sym_insert(char *name, VarType type, ASTNode *init, int is_temp) {
    for (int i = 0; i < sym_cnt; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return 0;               /* duplicate */
    if (sym_cnt >= MAX_SYMBOLS) {
        fprintf(stderr, "Symbol table overflow!\n");
        exit(1);
    }
    symtab[sym_cnt].name = strdup(name);
    symtab[sym_cnt].type = type;
    symtab[sym_cnt].init = init;
    symtab[sym_cnt].is_temp = is_temp;
    sym_cnt++;
    return 1;
}

Symbol* sym_lookup(char *name) {
    for (int i = 0; i < sym_cnt; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return &symtab[i];
    return NULL;
}

VarType sym_get_type(char *name) {
    Symbol *s = sym_lookup(name);
    return s ? s->type : VAR_INT;
}

/* ============================================================
   Type inference for AST nodes
   ============================================================ */
VarType node_type(ASTNode *n) {
    switch (n->kind) {
        case NODE_INT:   return VAR_INT;
        case NODE_FLOAT: return VAR_FLOAT;
        case NODE_ID:    return sym_get_type(n->d.sval);
        case NODE_ADD: {
            VarType t1 = node_type(n->d.bin.left);
            VarType t2 = node_type(n->d.bin.right);
            return (t1 == VAR_FLOAT || t2 == VAR_FLOAT) ? VAR_FLOAT : VAR_INT;
        }
        case NODE_LT:    return VAR_INT;  /* comparison yields int (boolean) */
        default:         return VAR_INT;
    }
}

/* ============================================================
   Three‑address code generation
   ============================================================ */
void emit_quad(OpCode op, char *dest, char *src1, char *src2, VarType type) {
    if (nquad >= MAX_QUADS) {
        fprintf(stderr, "Too many quads!\n");
        exit(1);
    }
    quads[nquad].op = op;
    quads[nquad].dest = dest ? strdup(dest) : NULL;
    quads[nquad].src1 = src1 ? strdup(src1) : NULL;
    quads[nquad].src2 = src2 ? strdup(src2) : NULL;
    quads[nquad].dest_type = type;
    nquad++;
}

char* new_temp(VarType type) {
    char buf[20];
    sprintf(buf, "_t%d", temp_counter++);
    sym_insert(buf, type, NULL, 1);  /* register as temporary */
    return strdup(buf);
}

char* new_label(void) {
    char buf[20];
    sprintf(buf, "L%d", label_counter++);
    return strdup(buf);
}

char* new_int_const(int val) {
    char buf[20];
    sprintf(buf, "_ci%d", nconst);
    const_pool[nconst].label = strdup(buf);
    const_pool[nconst].type = VAR_INT;
    const_pool[nconst].val.ival = val;
    nconst++;
    return strdup(buf);
}

char* new_float_const(float val) {
    char buf[20];
    sprintf(buf, "_cf%d", nconst);
    const_pool[nconst].label = strdup(buf);
    const_pool[nconst].type = VAR_FLOAT;
    const_pool[nconst].val.fval = val;
    nconst++;
    return strdup(buf);
}

char* gen_expr(ASTNode *node) {
    switch (node->kind) {
        case NODE_INT:   return new_int_const(node->d.ival);
        case NODE_FLOAT: return new_float_const(node->d.fval);
        case NODE_ID: {
            if (!sym_lookup(node->d.sval))
                return strdup("_error_");
            return strdup(node->d.sval);
        }
        case NODE_ADD: {
            char *left = gen_expr(node->d.bin.left);
            char *right = gen_expr(node->d.bin.right);
            VarType type = node_type(node);
            char *dest = new_temp(type);
            emit_quad(OP_ADD, dest, left, right, type);
            return dest;
        }
        case NODE_LT: {
            char *left = gen_expr(node->d.bin.left);
            char *right = gen_expr(node->d.bin.right);
            char *dest = new_temp(VAR_INT);
            emit_quad(OP_LT, dest, left, right, VAR_INT);
            return dest;
        }
        default:
            fprintf(stderr, "Internal error: bad expr node\n");
            return strdup("_error_");
    }
}

void gen_stmt(ASTNode *stmt) {
    if (!stmt) return;
    switch (stmt->kind) {
        case NODE_ASSIGN: {
            char *rhs = gen_expr(stmt->d.assign.expr);
            VarType type = sym_get_type(stmt->d.assign.name);
            emit_quad(OP_ASSIGN, stmt->d.assign.name, rhs, NULL, type);
            break;
        }
        case NODE_PRINTF: {
            ASTNode *expr = stmt->d.print.expr;
            char *arg = gen_expr(expr);
            if (strstr(stmt->d.print.fmt, "%d"))
                emit_quad(OP_PRINT_INT, NULL, arg, NULL, VAR_INT);
            else
                emit_quad(OP_PRINT_FLOAT, NULL, arg, NULL, VAR_FLOAT);
            break;
        }
        case NODE_BLOCK:
            for (int i = 0; i < stmt->d.block.cnt; i++)
                gen_stmt(stmt->d.block.stmts[i]);
            break;
        case NODE_LOOP: {
            char *start = new_label();
            emit_quad(OP_LABEL, start, NULL, NULL, 0);
            gen_stmt(stmt->d.loop.body);
            char *cond = gen_expr(stmt->d.loop.cond);
            emit_quad(OP_JMPTRUE, NULL, cond, start, 0);
            break;
        }
        default:
            fprintf(stderr, "Internal error: bad stmt node\n");
    }
}

void gen_program(ASTNode *root) {
    /* First, generate initialisation code for declared variables */
    for (int i = 0; i < sym_cnt; i++) {
        if (symtab[i].is_temp) continue;  /* skip temporaries */
        if (symtab[i].init) {
            char *rhs = gen_expr(symtab[i].init);
            emit_quad(OP_ASSIGN, symtab[i].name, rhs, NULL, symtab[i].type);
        } else {
            /* Default initialisation: 0 for int, 0.0 for float */
            if (symtab[i].type == VAR_INT)
                emit_quad(OP_ASSIGN, symtab[i].name, new_int_const(0), NULL, VAR_INT);
            else
                emit_quad(OP_ASSIGN, symtab[i].name, new_float_const(0.0f), NULL, VAR_FLOAT);
        }
    }
    /* Then the program body */
    gen_stmt(root);
    emit_quad(OP_HALT, NULL, NULL, NULL, 0);
}

/* ------------------------------------------------------------------
   AST visualisation – Graphviz DOT format
   ------------------------------------------------------------------ */
static int node_id_counter = 0;

static void ast_to_dot_rec(FILE *f, ASTNode *n, int parent_id) {
    if (!n) return;
    int my_id = node_id_counter++;
    const char *shape = "ellipse";
    char label[256] = "";

    switch (n->kind) {
        case NODE_INT:
            snprintf(label, sizeof(label), "INT(%d)", n->d.ival);
            shape = "box";
            break;
        case NODE_FLOAT:
            snprintf(label, sizeof(label), "FLOAT(%g)", n->d.fval);
            shape = "box";
            break;
        case NODE_ID:
            snprintf(label, sizeof(label), "ID(%s)", n->d.sval);
            shape = "box";
            break;
        case NODE_ADD:
            snprintf(label, sizeof(label), "ADD");
            break;
        case NODE_LT:
            snprintf(label, sizeof(label), "LT");
            break;
        case NODE_ASSIGN:
            snprintf(label, sizeof(label), "ASSIGN(%s)", n->d.assign.name);
            break;
        case NODE_PRINTF:
            snprintf(label, sizeof(label), "PRINTF(%s)", n->d.print.fmt);
            shape = "diamond";
            break;
        case NODE_BLOCK:
            snprintf(label, sizeof(label), "BLOCK[%d]", n->d.block.cnt);
            shape = "tab";
            break;
        case NODE_LOOP:
            snprintf(label, sizeof(label), "DO-WHILE");
            shape = "house";
            break;
        default:
            snprintf(label, sizeof(label), "???");
            break;
    }

    fprintf(f, "  n%d [label=\"%s\", shape=%s];\n", my_id, label, shape);
    if (parent_id >= 0)
        fprintf(f, "  n%d -> n%d;\n", parent_id, my_id);

    /* Add children according to node kind */
    switch (n->kind) {
        case NODE_ADD:
        case NODE_LT:
            ast_to_dot_rec(f, n->d.bin.left, my_id);
            ast_to_dot_rec(f, n->d.bin.right, my_id);
            break;
        case NODE_ASSIGN:
            ast_to_dot_rec(f, n->d.assign.expr, my_id);
            break;
        case NODE_PRINTF:
            ast_to_dot_rec(f, n->d.print.expr, my_id);
            break;
        case NODE_BLOCK:
            for (int i = 0; i < n->d.block.cnt; i++)
                ast_to_dot_rec(f, n->d.block.stmts[i], my_id);
            break;
        case NODE_LOOP:
            ast_to_dot_rec(f, n->d.loop.body, my_id);
            ast_to_dot_rec(f, n->d.loop.cond, my_id);
            break;
        default:
            break;  /* leaf nodes */
    }
}

void generate_ast_dot(ASTNode *root, const char *filename) {
    if (!root) return;
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror(filename);
        return;
    }
    node_id_counter = 0;
    fprintf(f, "digraph AST {\n");
    fprintf(f, "  node [fontname=\"Courier\"];\n");
    ast_to_dot_rec(f, root, -1);
    fprintf(f, "}\n");
    fclose(f);
    printf("AST written to %s\n", filename);
}

/* ============================================================
   Main driver – parse, generate TAC, call backend
   ============================================================ */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyin = f;
    yyparse();
    fclose(f);

    if (error_count > 0) {
        fprintf(stderr, "Compilation aborted due to errors.\n");
        return 1;
    }

    /* Generate TAC */
    gen_program(ast_root);

    /* Generate Graphviz .dot file */
    if (ast_root) {
        generate_ast_dot(ast_root, "ast.dot");
    }   

    /* Write emu8086 assembly */
    generate_emu8086("output.asm");

    return 0;
}