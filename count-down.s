;
; $Id: count-down.s,v 1.3 2016/09/23 17:33:46 urs Exp $
;

	.org $4000

cnt	= 0

	ldx #3
copy:	lda count,x
	sta cnt,x
	dex
	bpl copy

	bmi next	; N = 1
loop:	nop
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
	bne loop

end:	.byte $ff
	brk

count:	.byte 0,0,1,0
