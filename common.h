// common.h
#ifndef COMMON_H
#define COMMON_H

/* ---------- Enums ---------- */
typedef enum { VAR_INT, VAR_FLOAT } VarType;

typedef enum {
    OP_ADD, OP_LT, OP_ASSIGN,
    OP_PRINT_INT, OP_PRINT_FLOAT,
    OP_LABEL, OP_JMP, OP_JMPTRUE, OP_HALT
} OpCode;

/* ---------- Symbols ---------- */
typedef struct {
    char *name;
    VarType type;
    void *init;
    int is_temp;
} Symbol;

/* ---------- Constants ---------- */
typedef struct {
    char *label;
    VarType type;
    union {
        int ival;
        float fval;
    } val;
} Constant;

typedef struct {
    OpCode op;
    char *dest;
    char *src1;
    char *src2;
    VarType dest_type;      /* type of result (for ADD) */
} Quad;

#endif