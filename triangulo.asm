	TRIANGULO: EQU 1
SECTION TEXT
		INPUT		B 		;Comentario
		INPUT		H		;Comentario2
		LOAD		B
		MULT		H
		IF TRIANGULO		;IF
		DIV		DOIS
		STORE		R
		OUTPUT		R
		STOP				
SECTION BSS
	B:		SPACE 
	H:		SPACE
	R:		SPACE	
SECTION DATA
	DOIS:	CONST			2
