;
; $Id: demo.s,v 1.1 2016/09/23 21:55:05 urs Exp $
;

	.org $fe00

start:
; First, we execute some simple one-byte instructions.
	inx
	txa
	asl
	asl
	tay
	nop

; Next, we run a simple loop to show how the processor status register P
; is used by branch instructions.
loop1:	nop
	dey
	bne loop1

; Execute some instructions that set and use flags in the P register.
	sec
	rol
	sec
	rol

; Another loop using the carry flag C in register P.
loop2:	asl
	bcc loop2

; Load the X register with an immediate value $ff and copy to the stack
; pointer register S.
	ldx #$ff
	txs

; Then load X from memory location $ffff.
; This example shows that the same mnemonic (ldx in this case) can result
; in different instruction opcodes (A2 vs. AE), since we use different
; addressing modes.
	ldx $ffff

; Then load X from a memory location defined later and named 'data'.
; The assembler calculates the address of that memory location and
; puts it into the resulting object code.  This will load 42 into X.

; This produces the same opcode (AE) since it's the same addressing mode,
; namely absolute address.  It doesn't matter if we specify the address
; numerically or have the assembler calculate it for us.
	ldx data

; Jump forward to an absolute address, ...
	jmp fwd
	nop

back:

; Calculate the sum of 1...4 using a recursive routine
; and store the result in memory at address 0.
	ldx #4
	jsr sum
	sta 0

; Another way to do indirect jumps: Push the address (-1) to the stack
; then execute a rts instruction.  This is often used when we have a jump
; with a number of entries and an index into the table.  The 6502 doesn't
; offer this in one instruction, e.g. jmp (addr,x).

; First jump to 'back' again, i.e. entry number 1 in the jump table.
; One the next pass, jump to 'end', i.e. entry number 2 in the jump table.

	iny			; increment Y to 2, later to 4
	iny
	lda jt+1,y
	pha
	lda jt,y
	pha
	rts

; A recursive routine to add all integers from X to 0.
sum:	txa
	beq ret
	pha
	dex
	jsr sum
	tsx
	inx
	clc
	adc $0100,x
	txs
ret:	rts

end:	nop
	.byte $ff

data:	.byte 42

; ...then jump back through a jump table, i.e. using indirect addressing mode.
fwd:	jmp (jt)
	nop

jt:	.word back		; This is used with 'jmp (addr)'.
	.word back - 1		; These addresses are used with 'rts',
	.word end  - 1		; therefore, addresses are decremented by 1.

	.res $fffa - *
nmivec:	.word 0
resvec:	.word start
irqvec:	.word 0
