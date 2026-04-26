DATA SEGMENT
    ; User variables
    t0              DW 0

    ; Temporaries
    _t0             DW 0
    _t1             DW 0

    ; Constants
    _ci0            DW 1
    _ci1            DW 1
    _ci2            DW 0
    _ci3            DW 5
DATA ENDS

CODE SEGMENT
    ASSUME CS:CODE, DS:DATA
START:
    MOV AX, DATA
    MOV DS, AX
    MOV AX, [_ci0]
    MOV [t0], AX
L0:
    MOV AX, [t0]
    ADD AX, [_ci1]
    MOV [_t0], AX
    MOV AX, [_t0]
    MOV [t0], AX
    MOV AX, [t0]
    CALL PRINT_INT
    MOV AX, [_ci2]
    MOV [t0], AX
    MOV AX, [t0]
    CMP AX, [_ci3]
    JL L_true0
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
CODE ENDS
END START
