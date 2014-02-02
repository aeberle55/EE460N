; George Netscher and Austin Eberle
; Convert string from lower to upper case
	.ORIG	x3000
	LEA	R0, A		; R0 contains location of string starting address
	LDW R2, R0, #1	; R2 contains final address
	LDW	R0, R0, #0	; R0 contains string starting address
LOOP	LDB	R4, R0, #0	; load char into R4
	ADD	R4, R4, #-16	
	ADD	R4, R4, #-16	; subtract 32 to convert to uppercase
	BRN	DONE		; negative means null char
	STB	R4, R2, #0	; store byte in R4 at address in R2
	ADD	R0, R0, #1	; increment pointers
	ADD	R2, R2, #1	
	BRNZP	LOOP
DONE	AND	R4, R4, #0	; put null char in R4
	STB	R4, R2, #0	; append '/0' to output string
	HALT
A	.FILL	x4000
	.END
