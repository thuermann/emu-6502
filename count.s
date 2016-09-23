;
; $Id: count.s,v 1.3 2016/09/23 17:33:46 urs Exp $
;

	.org $4000

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

	.byte $ff
	brk
