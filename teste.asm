MOD_A: BEGIN
SECTION TEXT
N8: EXTERN
PUBLIC N9
ROT: INPUT N1
COPY     N1,        N8    ;comentario qualquer
ROT2: 
COPY    N4,    N8
COPY N3,   N3   +     1
OUTPUT N3 +    1
STOP
;comentario qualuer
SECTION DATA
N2:  CONST -5
SECTION BSS
N1:  SPACE
N3:  SPACE 2
N4:  SPACE
N9: SPACE 3
END