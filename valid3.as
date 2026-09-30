DATA1: .db 10, 20, 30
DATA2: .dw 1000, 2000
PROG: jmp PROG
	call FUNC
FUNC: sub $4, $5, $6
	hlt
