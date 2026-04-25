DATA SEGMENT
    ; User variables
    x               DW 0

    ; Temporaries
    _t0             DW 0
    _t1             DW 0

    ; Constants
    _ci0            DW 0
    _cf1            DB '3.14', '$'
    _cf2            DB '31.40', '$'
    _ci3            DW 12
    _ci4            DW 1
    _ci5            DW 5
DATA ENDS

CODE SEGMENT
    ASSUME CS:CODE, DS:DATA
START:
    MOV AX, DATA
    MOV DS, AX
    MOV AX, [_ci0]
    MOV [x], AX
L0:
    MOV DX, OFFSET _cf1
    MOV AH, 09h
    INT 21h
    MOV DL, 0Dh
    MOV AH, 2
    INT 21h
    MOV DL, 0Ah
    MOV AH, 2
    INT 21h
    MOV DX, OFFSET _cf2
    MOV AH, 09h
    INT 21h
    MOV DL, 0Dh
    MOV AH, 2
    INT 21h
    MOV DL, 0Ah
    MOV AH, 2
    INT 21h
    MOV AX, [_ci3]
    CALL PRINT_INT
    MOV AX, [x]
    CALL PRINT_INT
    MOV AX, [x]
    ADD AX, [_ci4]
    MOV [_t0], AX
    MOV AX, [_t0]
    MOV [x], AX
    MOV AX, [x]
    CMP AX, [_ci5]
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
