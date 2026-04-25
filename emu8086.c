#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    OP_ADD, OP_LT, OP_ASSIGN,
    OP_PRINT_INT, OP_PRINT_FLOAT,
    OP_LABEL, OP_JMP, OP_JMPTRUE, OP_HALT
} OpCode;

typedef enum { VAR_INT, VAR_FLOAT } VarType;

typedef struct {
    OpCode op;
    char *dest;
    char *src1;
    char *src2;
    VarType dest_type;
} Quad;

typedef struct {
    char *name;
    VarType type;
    int is_temp;
} Symbol;

typedef struct {
    char *label;
    VarType type;
    union {
        int ival;
        float fval;
    } val;
} Constant;

extern Quad quads[];
extern int nquad;
extern Symbol symtab[];
extern int sym_cnt;
extern Constant const_pool[];
extern int nconst;

static FILE *out;
static int lt_label_counter = 0;

static VarType sym_lookup_type(const char *name) {
    for (int i = 0; i < sym_cnt; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return symtab[i].type;
    return VAR_INT;
}

/* Returns 1 if the operand name represents a float value */
static int is_float_operand(const char *name) {
    if (!name) return 0;
    if (strncmp(name, "_cf", 2) == 0) return 1;
    /* look up in symbol table */
    for (int i = 0; i < sym_cnt; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return symtab[i].type == VAR_FLOAT;
    return 0;
}

static int has_float_operation(void) {
    for (int i = 0; i < nquad; i++) {
        Quad *q = &quads[i];
        if (q->op == OP_ADD && q->dest_type == VAR_FLOAT) return 1;
        if (q->op == OP_ASSIGN && q->dest_type == VAR_FLOAT) return 1;
        if (q->op == OP_LT) {
            if (q->src1 && (strncmp(q->src1, "_cf", 2) == 0 || sym_lookup_type(q->src1) == VAR_FLOAT)) return 1;
            if (q->src2 && (strncmp(q->src2, "_cf", 2) == 0 || sym_lookup_type(q->src2) == VAR_FLOAT)) return 1;
        }
        if (q->op == OP_PRINT_FLOAT) return 1;
    }
    return 0;
}

static void emit_data_section(FILE *f) {
    fprintf(f, "DATA SEGMENT\n");
    fprintf(f, "    ; User variables\n");
    for (int i = 0; i < sym_cnt; i++) {
        if (symtab[i].is_temp) continue;
        if (symtab[i].type == VAR_INT)
            fprintf(f, "    %-15s DW 0\n", symtab[i].name);
        else
            fprintf(f, "    %-15s DD 0.0\n", symtab[i].name);
    }

    fprintf(f, "\n    ; Temporaries\n");
    for (int i = 0; i < sym_cnt; i++) {
        if (!symtab[i].is_temp) continue;
        if (symtab[i].type == VAR_INT)
            fprintf(f, "    %-15s DW 0\n", symtab[i].name);
        else
            fprintf(f, "    %-15s DD 0.0\n", symtab[i].name);
    }

    fprintf(f, "\n    ; Constants\n");
    for (int i = 0; i < nconst; i++) {
        if (const_pool[i].type == VAR_INT)
            fprintf(f, "    %-15s DW %d\n", const_pool[i].label, const_pool[i].val.ival);
        else
            fprintf(f, "    %-15s DD %f\n", const_pool[i].label, const_pool[i].val.fval);
    }

    fprintf(f, "\n    ; Float‑printing helpers\n");
    fprintf(f, "    old_cw      DW ?\n");
    fprintf(f, "    new_cw      DW ?\n");
    fprintf(f, "    int_part    DW ?\n");
    fprintf(f, "    frac_part   DW ?\n");
    fprintf(f, "    const100    DW 100\n");
    fprintf(f, "    status_word DW ?\n");   /* for FSTSW */

    fprintf(f, "DATA ENDS\n\n");
}

static void emit_quad(FILE *f, Quad *q) {
    char *d = q->dest;
    char *s1 = q->src1;
    char *s2 = q->src2;

    switch (q->op) {
        case OP_LABEL:
            fprintf(f, "%s:\n", d);
            break;

        case OP_ASSIGN:
            if (q->dest_type == VAR_INT) {
                fprintf(f, "    MOV AX, [%s]\n", s1);
                fprintf(f, "    MOV [%s], AX\n", d);
            } else {  /* VAR_FLOAT */
                /* Load source: FLD for float, FILD for int */
                if (is_float_operand(s1))
                    fprintf(f, "    FLD DWORD PTR [%s]\n", s1);
                else
                    fprintf(f, "    FILD WORD PTR [%s]\n", s1);
                fprintf(f, "    FSTP DWORD PTR [%s]\n", d);
            }
            break;

        case OP_ADD:
            if (q->dest_type == VAR_INT) {
                fprintf(f, "    MOV AX, [%s]\n", s1);
                fprintf(f, "    ADD AX, [%s]\n", s2);
                fprintf(f, "    MOV [%s], AX\n", d);
            } else {   /* VAR_FLOAT */
                /* Load second operand */
                if (is_float_operand(s2))
                    fprintf(f, "    FLD DWORD PTR [%s]\n", s2);
                else
                    fprintf(f, "    FILD WORD PTR [%s]\n", s2);
                /* Load first operand */
                if (is_float_operand(s1))
                    fprintf(f, "    FLD DWORD PTR [%s]\n", s1);
                else
                    fprintf(f, "    FILD WORD PTR [%s]\n", s1);
                fprintf(f, "    FADD\n");
                fprintf(f, "    FSTP DWORD PTR [%s]\n", d);
            }
            break;

        case OP_LT: {
            int float_op = is_float_operand(s1) || is_float_operand(s2);

            if (!float_op) {
                fprintf(f, "    MOV AX, [%s]\n", s1);
                fprintf(f, "    CMP AX, [%s]\n", s2);
                fprintf(f, "    JL L_true%d\n", lt_label_counter);
                fprintf(f, "    MOV WORD PTR [%s], 0\n", d);
                fprintf(f, "    JMP L_end%d\n", lt_label_counter);
                fprintf(f, "L_true%d:\n", lt_label_counter);
                fprintf(f, "    MOV WORD PTR [%s], 1\n", d);
                fprintf(f, "L_end%d:\n", lt_label_counter);
            } else {
                /* Load second operand */
                if (is_float_operand(s2))
                    fprintf(f, "    FLD DWORD PTR [%s]\n", s2);
                else
                    fprintf(f, "    FILD WORD PTR [%s]\n", s2);
                /* Load first operand */
                if (is_float_operand(s1))
                    fprintf(f, "    FLD DWORD PTR [%s]\n", s1);
                else
                    fprintf(f, "    FILD WORD PTR [%s]\n", s1);
                fprintf(f, "    FCOMPP\n");
                fprintf(f, "    FSTSW WORD PTR [status_word]\n");
                fprintf(f, "    MOV AX, [status_word]\n");
                fprintf(f, "    SAHF\n");
                fprintf(f, "    JB L_true%d\n", lt_label_counter);
                fprintf(f, "    MOV WORD PTR [%s], 0\n", d);
                fprintf(f, "    JMP L_end%d\n", lt_label_counter);
                fprintf(f, "L_true%d:\n", lt_label_counter);
                fprintf(f, "    MOV WORD PTR [%s], 1\n", d);
                fprintf(f, "L_end%d:\n", lt_label_counter);
            }
            lt_label_counter++;
            break;
        }

        case OP_JMP:
            fprintf(f, "    JMP %s\n", s1 ? s1 : d);
            break;

        case OP_JMPTRUE:
            fprintf(f, "    MOV AX, [%s]\n", s1);
            fprintf(f, "    CMP AX, 1\n");
            fprintf(f, "    JE %s\n", s2);
            break;

        case OP_PRINT_INT:
            fprintf(f, "    MOV AX, [%s]\n", s1);
            fprintf(f, "    CALL PRINT_INT\n");
            break;

        case OP_PRINT_FLOAT:
            fprintf(f, "    LEA SI, [%s]\n", s1);
            fprintf(f, "    CALL PRINT_FLOAT\n");
            break;

        case OP_HALT:
            break;
    }
}

static void emit_code_section(FILE *f) {
    fprintf(f, "CODE SEGMENT\n");
    fprintf(f, "    .8087\n");
    fprintf(f, "    ASSUME CS:CODE, DS:DATA\n");
    fprintf(f, "START:\n");
    fprintf(f, "    MOV AX, DATA\n");
    fprintf(f, "    MOV DS, AX\n");

    if (has_float_operation()) {
        fprintf(f, "    FINIT\n");
    }

    for (int i = 0; i < nquad; i++) {
        emit_quad(f, &quads[i]);
    }

    fprintf(f, "    MOV AH, 4Ch\n");
    fprintf(f, "    INT 21h\n");

    fprintf(f, "\nPRINT_INT PROC NEAR\n");
    fprintf(f, "    PUSH AX\n");
    fprintf(f, "    PUSH BX\n");
    fprintf(f, "    PUSH CX\n");
    fprintf(f, "    PUSH DX\n");
    fprintf(f, "    MOV BX, 10\n");
    fprintf(f, "    XOR CX, CX\n");
    fprintf(f, "    CMP AX, 0\n");
    fprintf(f, "    JGE L1\n");
    fprintf(f, "    PUSH AX\n");
    fprintf(f, "    MOV DL, '-'\n");
    fprintf(f, "    MOV AH, 2\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    POP AX\n");
    fprintf(f, "    NEG AX\n");
    fprintf(f, "L1:\n");
    fprintf(f, "    XOR DX, DX\n");
    fprintf(f, "    DIV BX\n");
    fprintf(f, "    PUSH DX\n");
    fprintf(f, "    INC CX\n");
    fprintf(f, "    TEST AX, AX\n");
    fprintf(f, "    JNZ L1\n");
    fprintf(f, "L2:\n");
    fprintf(f, "    POP DX\n");
    fprintf(f, "    ADD DL, '0'\n");
    fprintf(f, "    MOV AH, 2\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    LOOP L2\n");
    fprintf(f, "    MOV DL, 0Dh\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    MOV DL, 0Ah\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    POP DX\n");
    fprintf(f, "    POP CX\n");
    fprintf(f, "    POP BX\n");
    fprintf(f, "    POP AX\n");
    fprintf(f, "    RET\n");
    fprintf(f, "PRINT_INT ENDP\n\n");

    fprintf(f, "PRINT_FLOAT PROC NEAR\n");
    fprintf(f, "    PUSH AX\n");
    fprintf(f, "    PUSH CX\n");
    fprintf(f, "    PUSH DX\n");
    fprintf(f, "    PUSH SI\n");
    fprintf(f, "    FLD DWORD PTR [SI]\n");
    fprintf(f, "    FST ST(1)\n");
    fprintf(f, "    FSTCW [old_cw]\n");
    fprintf(f, "    MOV AX, [old_cw]\n");
    fprintf(f, "    OR AX, 0C00h\n");
    fprintf(f, "    MOV [new_cw], AX\n");
    fprintf(f, "    FLDCW [new_cw]\n");
    fprintf(f, "    FISTP [int_part]\n");
    fprintf(f, "    FLDCW [old_cw]\n");
    fprintf(f, "    MOV AX, [int_part]\n");
    fprintf(f, "    CALL PRINT_INT\n");
    fprintf(f, "    MOV DL, '.'\n");
    fprintf(f, "    MOV AH, 2\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    FILD [int_part]\n");
    fprintf(f, "    FSUBP ST(1), ST(0)\n");
    fprintf(f, "    FIMUL [const100]\n");
    fprintf(f, "    FLDCW [new_cw]\n");
    fprintf(f, "    FISTP [frac_part]\n");
    fprintf(f, "    FLDCW [old_cw]\n");
    fprintf(f, "    MOV AX, [frac_part]\n");
    fprintf(f, "    CMP AX, 10\n");
    fprintf(f, "    JGE pf_no_lead\n");
    fprintf(f, "    PUSH AX\n");
    fprintf(f, "    MOV DL, '0'\n");
    fprintf(f, "    MOV AH, 2\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    POP AX\n");
    fprintf(f, "pf_no_lead:\n");
    fprintf(f, "    CALL PRINT_SMALL_INT\n");
    fprintf(f, "    MOV DL, 0Dh\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    MOV DL, 0Ah\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    POP SI\n");
    fprintf(f, "    POP DX\n");
    fprintf(f, "    POP CX\n");
    fprintf(f, "    POP AX\n");
    fprintf(f, "    RET\n");
    fprintf(f, "PRINT_FLOAT ENDP\n\n");

    fprintf(f, "PRINT_SMALL_INT PROC NEAR\n");
    fprintf(f, "    PUSH AX\n");
    fprintf(f, "    PUSH BX\n");
    fprintf(f, "    PUSH CX\n");
    fprintf(f, "    PUSH DX\n");
    fprintf(f, "    MOV BX, 10\n");
    fprintf(f, "    XOR CX, CX\n");
    fprintf(f, "psi_loop:\n");
    fprintf(f, "    XOR DX, DX\n");
    fprintf(f, "    DIV BX\n");
    fprintf(f, "    PUSH DX\n");
    fprintf(f, "    INC CX\n");
    fprintf(f, "    TEST AX, AX\n");
    fprintf(f, "    JNZ psi_loop\n");
    fprintf(f, "psi_print:\n");
    fprintf(f, "    POP DX\n");
    fprintf(f, "    ADD DL, '0'\n");
    fprintf(f, "    MOV AH, 2\n");
    fprintf(f, "    INT 21h\n");
    fprintf(f, "    LOOP psi_print\n");
    fprintf(f, "    POP DX\n");
    fprintf(f, "    POP CX\n");
    fprintf(f, "    POP BX\n");
    fprintf(f, "    POP AX\n");
    fprintf(f, "    RET\n");
    fprintf(f, "PRINT_SMALL_INT ENDP\n");

    fprintf(f, "CODE ENDS\n");
    fprintf(f, "END START\n");
}

void generate_emu8086(const char *filename) {
    out = fopen(filename, "w");
    if (!out) {
        perror(filename);
        exit(1);
    }

    emit_data_section(out);
    emit_code_section(out);

    fclose(out);
}