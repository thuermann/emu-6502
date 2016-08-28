;
; "$Id: test.s,v 1.1 2016/08/28 02:01:28 urs Exp $"
;
; Some dumb test code for the 6502 emulator.
; It's not very clever (i.e. fast and/or small) but tests many instructions,
; most addressing modes and setting/checking the CPU status flags.
;
; If everything runs correct, the following values shoud be in memory
; 0000 000013ba
; 0004 00000000
; 01ff d2
; 0400 ffffec46

	* = $4000

	ldx #$ff
	txs

; Test whether jsr/rts works correctly:
; Add 0..20 using recursion.
; Push the result to the stack so you can find it there in a memory dump.

	lda #20
	jsr sum
	pha
	jmp addsub

; sum(n) = if n == 0 then 0 else n + sum(n - 1)
sum:	tax
	beq end
	pha
	dex
	txa
	jsr sum
	tsx
	inx
	clc
	adc $0100,x
	txs
end:	rts

; Test adc and sbc, especially setting of C and V:
; Add 1..100 and subtract even numbers 2..200 from 0, then add both results.

addsub:	lda #0		; write 0 to 0..3 and 0400..0403
	ldx #3
loop0:	sta 0,x
	sta $0400,x
	dex
	bpl loop0

	ldy #200	; add all ints 1..100 to 0..3
loop1:	tsx 		; subtract all even ints 2..200 from 0400..0403
	tya
	pha
	cmp #101
	bcs noadd1	; skip if greater than 100

	adc 0		; straight forward 32 bit addition, C is 0
	sta 0
	lda #0
	adc 1
	sta 1
	lda #0
	adc 2
	sta 2
	lda #0
	adc 3
	sta 3
	bvs fail	; overflow shouldn't happen here

noadd1:	sec
	lda $0400
	sbc $0100,x
	ror
	bcs noadd2	; skip odd values
	rol 		; restore A and carry flag
	sta $0400
	ldy #1
	php
loop2:	plp		; get carry for this iteration
	lda $0400,y
	sbc #0
	sta $0400,y
	php		; save carry for the next iteration
	iny
	cpy #4
	bcc loop2
	plp		; restore carry and overflow flags from last subtraction
	bvs fail	; overflow shouldn't happen here

noadd2:	pla
	tay
	dey
	bne loop1

	asl 0		; divide and multiply sum by 2
	rol 1
	rol 2
	rol 3
	lsr 3
	ror 2
	ror 1
	ror 0

	lda $0403	; divide negative sum by 2
	rol
	php
	ror
	plp		; shift highest byte right by one bit
	ror		; keeping the most significant bit
	sta $0403
	ror $0402
	ror $0401
	ror $0400

src1	= $08
src2	= $0a
dst	= $0c
tmp	= $0e

	lda #0		; add 0..3 and 0400..0403 and store to 4..7
	sta src1	; use zpy addressing mode
	sta src1+1	; with zero page addr. 08/09, 0a/0b, and 0c/0d
	sta src2
	sta dst+1
	lda #4
	sta src2+1
	sta dst

	ldy #0
	sty tmp
	clc
	php
loop3:	plp
	lda (src1),y
	adc (src2),y
	sta (dst),y
	ora tmp		; OR of all bytes of the sum
	php
	iny
	cpy #4
	bcc loop3
	plp
	tax
	bne fail	; assert the sum is 0

	brk

fail:	jmp fail
