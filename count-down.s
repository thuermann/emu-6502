;
; $Id: count-down.s,v 1.4 2016/09/23 21:54:55 urs Exp $
;

	.org $ff00

cnt	= 0

; Copy the 32-bit counter start value to variable 'cnt' at addresses 0...3

start:	ldx #3
copy:	lda count,x
	sta cnt,x
	dex
	bpl copy

; Now decrement the 32-bit variable 'cnt' until it reaches 0.
; Register X is $ff and the N flag is set when we enter here.

	bmi next	; N = 1!
loop:	nop 		; This single NOP instruction is the loop body.
			; 'cnt' will iterate from 'count' - 1 to 0.
next:	dec cnt
	cpx cnt
	bne loop
	dec cnt+1
	cpx cnt+1
	bne loop
	dec cnt+2
	cpx cnt+2
	bne loop
	dec cnt+3
	cpx cnt+3
	bne loop	; If 'cnt' == $ffffffff ( = -1 ), exit the loop.

end:	.byte $ff

count:	.byte 0,0,1,0

	.res $fffa - *
nmivec:	.word 0
resvec:	.word start
irqvec:	.word 0
