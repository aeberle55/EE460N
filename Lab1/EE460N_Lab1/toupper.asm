; George Netscher and Austin Eberle
; Convert string from lower to upper case
	.ORIG	x3000
	LEA	R0, A		; R0 contains location of string starting address 	0xE00D
	LDW R2, R0, #1	; R2 contains final address							0x6401
	LDW	R0, R0, #0	; R0 contains string starting address				0x6000
LOOP	LDB	R4, R0, #0	; load char into R4; Location 0x3006 			0x2800
	ADD	R4, R4, #-16	;												0x1930
	ADD	R4, R4, #-16	; subtract 32 to convert to uppercase 			0x1930
	BRN	DONE		; negative means null char							0x0804
	STB	R4, R2, #0	; store byte in R4 at address in R2					0x3880
	ADD	R0, R0, #1	; increment pointers								0x1021
	ADD	R2, R2, #1	;													0x14A1
	BRNZP	LOOP	;													0x0FF8
DONE	AND	R4, R4, #0	; put null char in R4;							0x5920
	STB	R4, R2, #0	; append '/0' to output string;						0x3880
	HALT	;															0xF025
A	.FILL	x4000	;Location 0x301A, 26 above start					0x4000
	.END
