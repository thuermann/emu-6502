;
; "$Id: arith.s,v 1.5 2016/09/07 22:55:04 urs Exp $"
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
tmp	= $460

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

	lda #<s1	; src1 = &s1, src2 = &s2
	sta src1
	lda #>s1
	sta src1+1
	lda #<s2
	sta src2
	lda #>s2
	sta src2+1

; calculate s1 + s2
	lda #<sum	; dst = &sum, *dst = *src1 + *src2
	sta dst
	lda #>sum
	sta dst+1
	jsr adcx

; calculate s1 - s2
	lda #<diff	; dst = &diff, *dst = *src1 - *src2
	sta dst
	lda #>diff
	sta dst+1
	jsr sbcx

; calculate s1 << 4
	lda #<left	; dst = &left, *dst = *src1 << 4
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

; calculate s1 >> 4
	lda #<right	; dst = &right, *dst = *src1 >> 4
	sta dst
	lda #>right
	sta dst+1
	ldx #4
r1	clc
	jsr rorx
	dex
	bne r1

; calculate s1 * s2
	lda #<prod	; dst = &prod, *dst = *src1 * *src2
	sta dst
	lda #>prod
	sta dst+1
	jsr mulx

;  calculate base ^ 80
	lda #<pow	; dst = &pow, src1 = &base
	sta dst
	lda #>pow
	sta dst+1
	lda #<base
	sta src1
	lda #>base
	sta src1+1
	lda #<tmp
	sta src2
	lda #>tmp
	sta src2+1

	ldy cnt		; *dst = 1
	dey
c1	lda one,y
	sta (dst),y
	dey
	bpl c1

	ldx #80
lp1	txa
	pha

	ldy cnt		; *src2 = *dst
	dey
lp2	lda (dst),y
	sta (src2),y
	dey
	bpl lp2

	jsr mulx	; *dst = *src1 * *src2

	pla
	tax
	dex
	bne lp1

	brk

; copy *src1 to *dst

copyx	ldy cnt
	dey
copyx1	lda (src1),y
	sta (dst),y
	dey
	bpl copyx1
	rts

; rotate right
; *dst = (*src1 >> 1) | carry << (NUMBITS - 1), carry = *src1 & 1

rorx	ldy cnt
	dey
rorx1	lda (src1),y
	ror
	sta (dst),y
	dey
	bpl rorx1
	rts

; rotate left
; *dst = (*src1 << 1) | carry, carry = *src1 & (1 << NUMBITS-1)

rolx	ldy #0
	ldx cnt
rolx1	lda (src1),y
	rol
	sta (dst),y
	iny
	dex
	bne rolx1
	rts

; add
; *dst = *src1 + *src2

adcx	ldy #0
	ldx cnt
adcx1	lda (src1),y
	adc (src2),y
	sta (dst),y
	iny
	dex
	bne adcx1
	rts

; subtract
; *dst = *src1 - *src2

sbcx	ldy #0
	ldx cnt
sbcx1	lda (src1),y
	sbc (src2),y
	sta (dst),y
	iny
	dex
	bne sbcx1
	rts

; multiply
; *dst = *src1 * *src2

mulx	ldy cnt		; *dst = 0
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

	ldy cnt		; *src1 <<= 1
	dey
	lda (src1),y
	asl
	php
	ldy #0
	ldx cnt
mulxsr	lda (src1),y
	rol
	sta (src1),y
	iny
	dex
	bne mulxsr

	ldy #0		; *dst <<= 1
	ldx cnt
	clc
mulxsl	lda (dst),y
	rol
	sta (dst),y
	iny
	dex
	bne mulxsl

	plp
	bcc mulx2

	ldy #0		; *dst += *src2
	ldx cnt
	clc
mulxadd	lda (src2),y
	adc (dst),y
	sta (dst),y
	iny
	dex
	bne mulxadd

mulx2	pla
	sec
	sbc #1
	bne mulx1
	rts
