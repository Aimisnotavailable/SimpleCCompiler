#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"


extern Quad quads[];
extern int nquad;
extern Symbol symtab[];
extern int sym_cnt;
extern Constant const_pool[];
extern int nconst;

static FILE *out;
static int lt_label_counter = 0;

/* ------------------------------------------------------------------ */
/*  Emit the data section – only read symtab, never write it           */
/* ------------------------------------------------------------------ */
static void emit_data_section(FILE *f) {
    fprintf(f, "DATA SEGMENT\n");

    fprintf(f, "    ; User variables\n");
    for (int i = 0; i < sym_cnt; i++) {
        if (symtab[i].is_temp) continue;
        if (symtab[i].name == NULL || symtab[i].name[0] == '\0') continue;
        if (symtab[i].type == VAR_INT)
            fprintf(f, "    %-15s DW 0\n", symtab[i].name);
        /* Float user variables are unsupported → simply skip */
    }

    fprintf(f, "\n    ; Temporaries\n");
    for (int i = 0; i < sym_cnt; i++) {
        if (!symtab[i].is_temp) continue;
        if (symtab[i].name == NULL || symtab[i].name[0] == '\0') continue;
        if (symtab[i].type == VAR_INT)
            fprintf(f, "    %-15s DW 0\n", symtab[i].name);
    }

    fprintf(f, "\n    ; Constants\n");
    for (int i = 0; i < nconst; i++) {
        if (const_pool[i].type == VAR_INT)
            fprintf(f, "    %-15s DW %d\n",
                    const_pool[i].label, const_pool[i].val.ival);
        else {  /* VAR_FLOAT → string constant */
            char buf[64];
            sprintf(buf, "%.2f", const_pool[i].val.fval);
            fprintf(f, "    %-15s DB '%s', '$'\n",
                    const_pool[i].label, buf);
        }
    }

    fprintf(f, "DATA ENDS\n\n");
}

/* ------------------------------------------------------------------ */
/*  Emit code for one quadruple (reads quads, never touches symtab)    */
/* ------------------------------------------------------------------ */
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
            } else {
                /* Float assignment not implemented */
                fprintf(f, "    ; float assign ignored\n");
            }
            break;

        case OP_ADD:
            if (q->dest_type == VAR_INT) {
                fprintf(f, "    MOV AX, [%s]\n", s1);
                fprintf(f, "    ADD AX, [%s]\n", s2);
                fprintf(f, "    MOV [%s], AX\n", d);
            } else {
                fprintf(f, "    ; float add ignored\n");
            }
            break;

        case OP_LT: {
            /* Integer comparison only */
            fprintf(f, "    MOV AX, [%s]\n", s1);
            fprintf(f, "    CMP AX, [%s]\n", s2);
            fprintf(f, "    JL L_true%d\n", lt_label_counter);
            fprintf(f, "    MOV WORD PTR [%s], 0\n", d);
            fprintf(f, "    JMP L_end%d\n", lt_label_counter);
            fprintf(f, "L_true%d:\n", lt_label_counter);
            fprintf(f, "    MOV WORD PTR [%s], 1\n", d);
            fprintf(f, "L_end%d:\n", lt_label_counter);
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
            fprintf(f, "    MOV DX, OFFSET %s\n", s1);
            fprintf(f, "    MOV AH, 09h\n");
            fprintf(f, "    INT 21h\n");
            fprintf(f, "    MOV DL, 0Dh\n");
            fprintf(f, "    MOV AH, 2\n");
            fprintf(f, "    INT 21h\n");
            fprintf(f, "    MOV DL, 0Ah\n");
            fprintf(f, "    MOV AH, 2\n");
            fprintf(f, "    INT 21h\n");
            break;

        case OP_HALT:
            break;
    }
}

/* ------------------------------------------------------------------ */
/*  Emit the code section                                              */
/* ------------------------------------------------------------------ */
static void emit_code_section(FILE *f) {
    fprintf(f, "CODE SEGMENT\n");
    fprintf(f, "    ASSUME CS:CODE, DS:DATA\n");
    fprintf(f, "START:\n");
    fprintf(f, "    MOV AX, DATA\n");
    fprintf(f, "    MOV DS, AX\n");

    for (int i = 0; i < nquad; i++)
        emit_quad(f, &quads[i]);

    fprintf(f, "    MOV AH, 4Ch\n");
    fprintf(f, "    INT 21h\n");

    /* PRINT_INT (unchanged) */
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
    fprintf(f, "PRINT_INT ENDP\n");
    fprintf(f, "CODE ENDS\n");
    fprintf(f, "END START\n");
}

/* ------------------------------------------------------------------ */
/*  Public entry point – no writes to symtab                           */
/* ------------------------------------------------------------------ */
void generate_emu8086(const char *filename) {
    out = fopen(filename, "w");
    if (!out) {
        perror(filename);
        exit(1);
    }

    // Optionally verify symtab at entry:
    printf("Inside generate_emu8086, symtab:\n");
    for (int i = 0; i < sym_cnt; i++)
        printf("  [%d] %s\n", i, symtab[i].name);

    emit_data_section(out);
    emit_code_section(out);

    fclose(out);
}