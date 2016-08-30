;
; "$Id: count-down.s,v 1.1 2016/08/30 13:59:47 urs Exp $"
;

	* = $4000

cnt	= 0

start	ldx #3
copy	lda count,x
	sta cnt,x
	dex
	bpl copy

	bmi next	; N = 1
loop	nop
next	dec cnt
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

end	brk

count	.byte 0,0,1,0
