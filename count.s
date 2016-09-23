;
; $Id: count.s,v 1.4 2016/09/23 21:54:55 urs Exp $
;

	.org $ff00

; Store the 32-bit integer 0 into memory at addresses 0...3

start:	lda #0
	sta 0
	sta 1
	sta 2
	sta 3

; Then increment that 32-bit integer by until it reaches 0 again,
; .i.e. 2^32 times.

loop:	inc 0
	bne loop
	inc 1
	bne loop
	inc 2
	bne loop
	inc 3
	bne loop

end:	.byte $ff

	.res $fffa - *
nmivec:	.word 0
resvec:	.word start
irqvec:	.word 0
