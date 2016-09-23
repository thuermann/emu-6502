//
// $Id: observer.cc,v 1.5 2016/09/23 00:13:36 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>

#include "cpu6502.hh"

static void disas(uint8_t *opc);

void cpu_6502::observe_init()
{
    icount = fcount = mcount = 0;
    std::cout << std::hex << std::setfill('0');
}

void cpu_6502::observe_finish()
{
    if (verbose > 0) {
	observe_prt_cpu_state();
	std::cout << '\n';
    }
    std::cout << std::dec << icount << " instructions executed, "
	      << fcount << " opcode bytes fetched.\n"
	      << mcount - fcount << " memory data accesses."
	      << std::endl;
}

void cpu_6502::observe_mem_access(uint16_t addr, enum dir d, uint8_t val)
{
    mcount++;

    int i = mlen++;
    maccess[i].addr = addr, maccess[i].dir = d, maccess[i].val = val;
}

void cpu_6502::observe_fetch(uint8_t opcode)
{
    fcount++;

    opc[opclen++] = opcode;
}

void cpu_6502::observe_begin()
{
    opclen = mlen = 0;
    if (verbose > 0) {
	observe_prt_cpu_state();
	std::cout << "\texecuting " << std::setw(4) << PC << ": ";
    }
}

void cpu_6502::observe_end()
{
    icount++;

    if (verbose > 0) {
	for (int i = 0; i < 3; i++)
	    if (i < opclen)
		std::cout << ' ' << std::setw(2) << (int)opc[i];
	    else
		std::cout << "   ";
	std::cout << "  ";
	disas(opc);
	std::cout << std::endl;
	if (verbose > 1) {
	    for (int i = 0; i < mlen; i++)
		std::cout << "\t\t\t"
			  << std::setw(4) << maccess[i].addr << ' '
			  << maccess[i].dir["RW"] << ' '
			  << std::setw(2) << (int)maccess[i].val
			  << std::endl;
	}
    }
}

void cpu_6502::observe_prt_cpu_state()
{
    std::cout << "["
	      << std::setw(2) << (int)A << ' '
	      << std::setw(2) << (int)X << ' '
	      << std::setw(2) << (int)Y << ' '
	      << std::setw(2) << (int)S << ' '
	      << std::setw(2) << (int)P << ' '
	      << std::setw(4) << (int)PC << ']';
}

enum { INX, ZP, IMM, ABS, INY, ZPX, ABY, ABX, ZPY, IND, REL };

#define II(mnc)     { #mnc, -1 }
#define IA(mnc, am) { #mnc, am }
#define ILL         II()

static const struct {
    char mnc[4];
    int am;
} itab[] = {
    IA(brk,  -1), IA(ora, INX), ILL,          ILL,
    ILL,          IA(ora,  ZP), IA(asl,  ZP), ILL,
    II(php),      IA(ora, IMM), II(asl),      ILL,
    ILL,          IA(ora, ABS), IA(asl, ABS), ILL,

    IA(bpl, REL), IA(ora, INY), ILL,          ILL,
    ILL,          IA(ora, ZPX), IA(asl, ZPX), ILL,
    II(clc),      IA(ora, ABY), ILL,          ILL,
    ILL,          IA(ora, ABX), IA(asl, ABX), ILL,

    IA(jsr, ABS), IA(and, INX), ILL,          ILL,
    II(bit),      IA(and,  ZP), IA(rol,  ZP), ILL,
    II(plp),      IA(and, IMM), II(rol),      ILL,
    II(bit),      IA(and, ABS), IA(rol, ABS), ILL,

    IA(bmi, REL), IA(and, INY), ILL,          ILL,
    ILL,          IA(and, ZPX), IA(rol, ZPX), ILL,
    II(sec),      IA(and, ABY), ILL,          ILL,
    ILL,          IA(and, ABX), IA(rol, ABX), ILL,

    II(rti),      IA(eor, INX), ILL,          ILL,
    ILL,          IA(eor,  ZP), IA(lsr,  ZP), ILL,
    II(pha),      IA(eor, IMM), II(lsr),      ILL,
    IA(jmp, ABS), IA(eor, ABS), IA(lsr, ABS), ILL,

    IA(bvc, REL), IA(eor, INY), ILL,          ILL,
    ILL,          IA(eor, ZPX), IA(lsr, ZPX), ILL,
    II(cli),      IA(eor, ABY), ILL,          ILL,
    ILL,          IA(eor, ABX), IA(lsr, ABX), ILL,

    II(rts),      IA(adc, INX), ILL,          ILL,
    ILL,          IA(adc,  ZP), IA(ror,  ZP), ILL,
    II(pla),      IA(adc, IMM), II(ror),      ILL,
    IA(jmp, IND), IA(adc, ABS), IA(ror, ABS), ILL,

    IA(bvs, REL), IA(adc, INY), ILL,          ILL,
    ILL,          IA(adc, ZPX), IA(ror, ZPX), ILL,
    II(sei),      IA(adc, ABY), ILL,          ILL,
    ILL,          IA(adc, ABX), IA(ror, ABX), ILL,

    ILL,          IA(sta, INX), ILL,          ILL,
    IA(sty,  ZP), IA(sta,  ZP), IA(stx, ZP),  ILL,
    II(dey),      IA(sta, IMM), II(txa),      ILL,
    IA(sty, ABS), IA(sta, ABS), IA(stx, ABS), ILL,

    IA(bcc, REL), IA(sta, INY), ILL,          ILL,
    IA(sty, ZPX), IA(sta, ZPX), IA(stx, ZPY), ILL,
    II(tya),      IA(sta, ABY), II(txs),      ILL,
    ILL,          IA(sta, ABX), ILL,          ILL,

    IA(ldy, IMM), IA(lda, INX), IA(ldx, IMM), ILL,
    IA(ldy,  ZP), IA(lda,  ZP), IA(ldx,  ZP), ILL,
    II(tay),      IA(lda, IMM), II(tax),      ILL,
    IA(ldy, ABS), IA(lda, ABS), IA(ldx, ABS), ILL,

    IA(bcs, REL), IA(lda, INY), ILL,          ILL,
    IA(ldy, ZPX), IA(lda, ZPX), IA(ldx, ZPY), ILL,
    II(clv),      IA(lda, ABY), II(tsx),      ILL,
    IA(ldy, ABX), IA(lda, ABX), IA(ldx, ABY), ILL,

    IA(cpy, IMM), IA(cmp, INX), ILL,          ILL,
    IA(cpy,  ZP), IA(cmp,  ZP), IA(dec,  ZP), ILL,
    II(iny),      IA(cmp, IMM), II(dex),      ILL,
    IA(cpy, ABS), IA(cmp, ABS), IA(dec, ABS), ILL,

    IA(bne, REL), IA(cmp, INY), ILL,          ILL,
    ILL,          IA(cmp, ZPX), IA(dec, ZPX), ILL,
    II(cld),      IA(cmp, ABY), ILL,          ILL,
    ILL,          IA(cmp, ABX), IA(dec, ABX), ILL,

    IA(cpx, IMM), IA(sbc, INX), ILL,          ILL,
    IA(cpx,  ZP), IA(sbc,  ZP), IA(inc,  ZP), ILL,
    II(inx),      IA(sbc, IMM), II(nop),      ILL,
    IA(cpx, ABS), IA(sbc, ABS), IA(inc, ABS), ILL,

    IA(beq, REL), IA(sbc, INY), ILL,          ILL,
    ILL,          IA(sbc, ZPX), IA(inc, ZPX), ILL,
    II(sed),      IA(sbc, ABY), ILL,          ILL,
    ILL,          IA(sbc, ABX), IA(inc, ABX), ILL,
};

#define ADDR(lo, hi) ((uint16_t)(hi) << 8 | (lo))

static void disas(uint8_t *opc)
{
    if (opc[0] >= sizeof(itab) / sizeof(itab[0]))
	return;

    std::cout << itab[opc[0]].mnc << ' ';
    switch (itab[opc[0]].am) {
    case INX:
	std::cout << '(' << std::setw(2) << (int)opc[1] << ",x)";
	break;
    case INY:
	std::cout << '(' << std::setw(2) << (int)opc[1] << "),y";
	break;
    case REL:
    case ZP:
	std::cout << std::setw(2) << (int)opc[1];
	break;
    case ZPX:
	std::cout << std::setw(2) << (int)opc[1] << ",x";
	break;
    case ZPY:
	std::cout << std::setw(2) << (int)opc[1] << ",y";
	break;
    case ABS:
	std::cout << std::setw(4) << ADDR(opc[1], opc[2]);
	break;
    case ABX:
	std::cout << std::setw(4) << ADDR(opc[1], opc[2]) << ",x";
	break;
    case ABY:
	std::cout << std::setw(4) << ADDR(opc[1], opc[2]) << ",y";
	break;
    case IND:
	std::cout << '(' << std::setw(4) << ADDR(opc[1], opc[2]) << ')';
	break;
    case IMM:
	std::cout << '#' << std::setw(2) << (int)opc[1];
	break;
    }
}
