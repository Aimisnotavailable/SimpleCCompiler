DATA SEGMENT
    ; User variables
    x               DW 0
                    DW 0
    _t0             DW 0

    ; Temporaries

    ; Constants
    _ci0            DW 0
    _cf1            DD 3.140000
    _cf2            DD 31.400000
    _ci3            DW 1
    _ci4            DW 5

    ; Float‑printing helpers
    old_cw      DW ?
    new_cw      DW ?
    int_part    DW ?
    frac_part   DW ?
    const100    DW 100
    status_word DW ?
DATA ENDS

CODE SEGMENT
    .8087
    ASSUME CS:CODE, DS:DATA
START:
    MOV AX, DATA
    MOV DS, AX
    FINIT
    MOV AX, [_ci0]
    MOV [x], AX
L0:
    LEA SI, [_cf1]
    CALL PRINT_FLOAT
    LEA SI, [_cf2]
    CALL PRINT_FLOAT
    MOV AX, [x]
    ADD AX, [_ci3]
    MOV [_t0], AX
    MOV AX, [_t0]
    MOV [x], AX
    FLD DWORD PTR [_ci4]
    FILD WORD PTR [x]
    FCOMPP
    FSTSW WORD PTR [status_word]
    MOV AX, [status_word]
    SAHF
    JB L_true0
    MOV WORD PTR [_t1], 0
    JMP L_end0
L_true0:
    MOV WORD PTR [_t1], 1
L_end0:
    MOV AX, [_t1]
    CMP AX, 1
    JE L0
    MOV AH, 4Ch
    INT 21h

PRINT_INT PROC NEAR
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    MOV BX, 10
    XOR CX, CX
    CMP AX, 0
    JGE L1
    PUSH AX
    MOV DL, '-'
    MOV AH, 2
    INT 21h
    POP AX
    NEG AX
L1:
    XOR DX, DX
    DIV BX
    PUSH DX
    INC CX
    TEST AX, AX
    JNZ L1
L2:
    POP DX
    ADD DL, '0'
    MOV AH, 2
    INT 21h
    LOOP L2
    MOV DL, 0Dh
    INT 21h
    MOV DL, 0Ah
    INT 21h
    POP DX
    POP CX
    POP BX
    POP AX
    RET
PRINT_INT ENDP

PRINT_FLOAT PROC NEAR
    PUSH AX
    PUSH CX
    PUSH DX
    PUSH SI
    FLD DWORD PTR [SI]
    FST ST(1)
    FSTCW [old_cw]
    MOV AX, [old_cw]
    OR AX, 0C00h
    MOV [new_cw], AX
    FLDCW [new_cw]
    FISTP [int_part]
    FLDCW [old_cw]
    MOV AX, [int_part]
    CALL PRINT_INT
    MOV DL, '.'
    MOV AH, 2
    INT 21h
    FILD [int_part]
    FSUBP ST(1), ST(0)
    FIMUL [const100]
    FLDCW [new_cw]
    FISTP [frac_part]
    FLDCW [old_cw]
    MOV AX, [frac_part]
    CMP AX, 10
    JGE pf_no_lead
    PUSH AX
    MOV DL, '0'
    MOV AH, 2
    INT 21h
    POP AX
pf_no_lead:
    CALL PRINT_SMALL_INT
    MOV DL, 0Dh
    INT 21h
    MOV DL, 0Ah
    INT 21h
    POP SI
    POP DX
    POP CX
    POP AX
    RET
PRINT_FLOAT ENDP

PRINT_SMALL_INT PROC NEAR
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    MOV BX, 10
    XOR CX, CX
psi_loop:
    XOR DX, DX
    DIV BX
    PUSH DX
    INC CX
    TEST AX, AX
    JNZ psi_loop
psi_print:
    POP DX
    ADD DL, '0'
    MOV AH, 2
    INT 21h
    LOOP psi_print
    POP DX
    POP CX
    POP BX
    POP AX
    RET
PRINT_SMALL_INT ENDP
CODE ENDS
END START
