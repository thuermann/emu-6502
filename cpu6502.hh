//
// $Id: cpu6502.hh,v 1.7 2016/09/19 23:00:17 urs Exp $
//

#ifndef CPU6502_HH
#define CPU6502_HH

#include <cstdint>

#include "memory.hh"

class cpu_6502 {
public:
    cpu_6502() : A(0), X(0), Y(0), S(0), P(0), PC(0),
		 verbose(false) {}
    void attach(memory *mem) {
	this->mem.attach(mem);
    }
    void reset();
    void run();

    void set_verbose(bool v) { verbose = v; }

private:
    // The CPU register set
    uint8_t  A, X, Y, S, P;
    uint16_t PC;
    enum { C, Z, I, D, U1, U2, V, N };
    void set(int bit, bool b) {
	if (b)
	    P |= 1 << bit;
	else
	    P &= ~(1 << bit);
    }
    bool get(int bit) {
	return P & (1 << bit);
    }

    const uint16_t NMI   = 0xfffa;
    const uint16_t RESET = 0xfffc;
    const uint16_t IRQ   = 0xfffe;

    // External memory (64K)
    class mem_interface {
    public:
	void attach(memory *mem)                  { this->mem = mem; }
	uint8_t load(uint16_t addr)               { return mem->load(addr); }
	void    store(uint16_t addr, uint8_t val) { mem->store(addr, val); }
    private:
	memory *mem;
    } mem;

    // CPU instructions
    void sta(uint8_t opcode);
    void lda(uint8_t opcode);
    void ora(uint8_t opcode);
    void AND(uint8_t opcode);	// what drugs have the C++ designers been on?
    void eor(uint8_t opcode);
    void adc(uint8_t opcode);
    void sbc(uint8_t opcode);
    void cmp(uint8_t opcode);

    void stx(uint8_t opcode);
    void ldx(uint8_t opcode);
    void cpx(uint8_t opcode);

    void sty(uint8_t opcode);
    void ldy(uint8_t opcode);
    void cpy(uint8_t opcode);

    void pha(uint8_t opcode);
    void pla(uint8_t opcode);
    void php(uint8_t opcode);
    void plp(uint8_t opcode);
    void tax(uint8_t opcode);
    void txa(uint8_t opcode);
    void tay(uint8_t opcode);
    void tya(uint8_t opcode);
    void txs(uint8_t opcode);
    void tsx(uint8_t opcode);
    void inx(uint8_t opcode);
    void iny(uint8_t opcode);
    void dex(uint8_t opcode);
    void dey(uint8_t opcode);
    void inc(uint8_t opcode);
    void dec(uint8_t opcode);

    void asl(uint8_t opcode);
    void lsr(uint8_t opcode);
    void rol(uint8_t opcode);
    void ror(uint8_t opcode);

    void bit(uint8_t opcode);
    void clF(uint8_t opcode);
    void seF(uint8_t opcode);
    void bCC(uint8_t opcode);
    void jmp(uint8_t opcode);
    void jsr(uint8_t opcode);
    void rts(uint8_t opcode);
    void brk(uint8_t opcode);
    void rti(uint8_t opcode);
    void nop(uint8_t opcode);

    // Auxilliary functions
    uint8_t fetch();

    void    push(uint8_t val);
    uint8_t pull();

    enum { INX, ZP, IMM, ABS, INY, ZPX, ABY, ABX, ZPY, IND };
    struct ea {
	enum { MEM, IMM } type;
	union {
	    uint8_t  val;
	    uint16_t addr;
	};
    };
    struct ea get_ea(uint8_t addrmode);
    uint8_t   load_ea(struct ea ea);
    void      store_ea(struct ea ea, uint8_t val);

    // ALU functions
    void alu_adc(uint8_t val);
    void alu_sbc(uint8_t val);
    void alu_and(uint8_t val);
    void alu_ora(uint8_t val);
    void alu_eor(uint8_t val);
    void alu_cmp(uint8_t val1, uint8_t val2);
    uint8_t alu_asl(uint8_t val);
    uint8_t alu_lsr(uint8_t val);
    uint8_t alu_rol(uint8_t val);
    uint8_t alu_ror(uint8_t val);
    uint8_t alu_inc(uint8_t val);
    uint8_t alu_dec(uint8_t val);

    void set_NZ(uint8_t val);

    // The instruction decoder table
    typedef void (cpu_6502::*instruction)(uint8_t opcode);
    static const instruction itab[256];

    // Reporting CPU actions
    bool verbose;
    uint8_t opc[3];
    int opclen;
};

#endif
