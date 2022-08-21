K:		.data 22
MAIN:	mov S1.1, W
		add r2,STR
S2:		.struct -1	 , 	 "12345"

XYZ: 	jmp W
		macro m1
		inc K
; This is comment!!!
		mov S1.2 ,	r3	
		endmacro
		prn 	#-50

		sub r1	, r4
		m1
		bne L3
END:	hlt
STR:	.string "abcdef"
LENGTH:	.data 6,-512,511
S1:		.struct 8, "ab"
		jmp LOOP
.entry K
.entry XYZ
.extern L3
.extern W
.extern LOOP