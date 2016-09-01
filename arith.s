;
; "$Id: arith.s,v 1.3 2016/09/01 07:30:28 urs Exp $"
;

	* = $4000

src1	= $20
src2	= $22
dst	= $24
cnt	= $26

sum	= $400
diff	= $410
left	= $420
right	= $430
prod	= $440
pow	= $450

entry	jmp start
	nop

s1	.byte $4e, $61, $bc, $00,0,0,0,0,0,0,0,0,0,0,0,0
s2	.byte $b1, $7f, $39, $05,0,0,0,0,0,0,0,0,0,0,0,0

base	.byte $67,$45,$23,$01,$98,$ba,$dc,$fe,0,0,0,0,0,0,0,0
one	.byte 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

start	ldx #$ff
	txs

	lda #16
	sta cnt

	lda #<s1
	sta src1
	lda #>s1
	sta src1+1
	lda #<s2
	sta src2
	lda #>s2
	sta src2+1

	lda #<sum
	sta dst
	lda #>sum
	sta dst+1
	jsr adcx

	lda #<diff
	sta dst
	lda #>diff
	sta dst+1
	jsr sbcx

	lda #<left
	sta dst
	lda #>left
	sta dst+1
	lda #4
l1	pha
	clc
	jsr rolx
	pla
	sec
	sbc #1
	bne l1

	lda #<right
	sta dst
	lda #>right
	sta dst+1
	ldx #4
r1	clc
	jsr rorx
	dex
	bne r1

	lda #<prod
	sta dst
	lda #>prod
	sta dst+1
	jsr mulx

	lda #<pow
	sta dst
	lda #>pow
	sta dst+1
	lda #<base
	sta src1
	lda #>base
	sta src1+1

	ldy cnt
	dey
c1	lda one,y
	sta (dst),y
	dey
	bpl c1

	ldx #80
lp1	txa
	pha

	ldy cnt
	dey
lp2	lda (dst),y
	sta (src2),y
	dey
	bpl lp2

	jsr mulx

	pla
	tax
	dex
	bne lp1

	brk

copyx	ldy cnt
	dey
copyx1	lda (src1),y
	sta (dst),y
	dey
	bpl copyx1
	rts

rorx	ldy cnt
	dey
rorx1	lda (src1),y
	ror
	sta (dst),y
	dey
	bpl rorx1
	rts

rolx	ldy #0
	ldx cnt
rolx1	lda (src1),y
	rol
	sta (dst),y
	iny
	dex
	bne rolx1
	rts

adcx	ldy #0
	ldx cnt
adcx1	lda (src1),y
	adc (src2),y
	sta (dst),y
	iny
	dex
	bne adcx1
	rts

sbcx	ldy #0
	ldx cnt
sbcx1	lda (src1),y
	sbc (src2),y
	sta (dst),y
	iny
	dex
	bne sbcx1
	rts

mulx	ldy cnt
	dey
	lda #0
mulx0	sta (dst),y
	dey
	bpl mulx0

	lda cnt
	asl
	asl
	asl
mulx1	pha

	ldy cnt		; lsrx src1
	dey
mulxsr	lda (src1),y
	ror
	sta (src1),y
	dey
	bpl mulxsr

	bcc mulx2

	ldy #0		; dst += src2
	ldx cnt
	clc
mulxadd	lda (src2),y
	adc (dst),y
	sta (dst),y
	iny
	dex
	bne mulxadd

mulx2	ldy #0		; shift src2
	ldx cnt
	clc
mulxsl	lda (src2),y
	rol
	sta (src2),y
	iny
	dex
	bne mulxsl

	pla
	sec
	sbc #1
	bne mulx1
	rts
