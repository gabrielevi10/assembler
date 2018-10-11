SECTION TEXT
        INPUT N
        LOAD N
FAT:    SUB ONE
        JMPZ FIM
        STORE AUX
        MULT N
        STORE N
        LOAD AUX
        JMP FAT
FIM:    OUTPUT N
        STOP
SECTION BSS
AUX:    SPACE 1
N:      SPACE 1
SECTION DATA
ONE:    CONST 1