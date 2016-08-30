;
; "$Id: count.s,v 1.1 2016/08/30 13:59:47 urs Exp $"
;

	* = $4000

	lda #0
	sta 0
	sta 1
	sta 2
	sta 3

loop:	inc 0
	bne loop
	inc 1
	bne loop
	inc 2
	bne loop
	inc 3
	bne loop

	brk
