//
// $Id: cpu6502.cc,v 1.4 2016/08/31 05:34:59 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>

#include "cpu6502.hh"
#include "memory.hh"


void cpu_6502::reset()
{
    uint8_t lo = mem->load(RESET);
    uint8_t hi = mem->load(RESET + 1);
    PC = (hi << 8) | lo;
    run();
}

void cpu_6502::run()
{
    std::cout << std::hex << std::setfill('0');
    while (1) {
	if (verbose)
	    std::cout << "executing " << std::setw(4) << PC << ": ";
	uint8_t opcode = fetch();
	(this->*itab[opcode])(opcode);
	if (verbose)
	    std::cout << std::setw(2) << (int)opcode << "\t["
		      << std::setw(2) << (int)A << ' '
		      << std::setw(2) << (int)X << ' '
		      << std::setw(2) << (int)Y << ' '
		      << std::setw(2) << (int)S << ' '
		      << std::setw(2) << (int)P << ' '
		      << std::setw(4) << (int)PC << ']'
		      << std::endl;
	// Temporary hack to provide a way to terminate a program
	if (opcode == 0)
	    break;
    }
}

#define I(mnc) &cpu_6502::mnc

const cpu_6502::instruction cpu_6502::itab[256] = {
    I(brk), I(ora),      0, 0,      0, I(ora), I(asl), 0,  // 00
    I(php), I(ora), I(asl), 0,      0, I(ora), I(asl), 0,  // 08
    I(bCC), I(ora),      0, 0,      0, I(ora), I(asl), 0,  // 10
    I(clF), I(ora),      0, 0,      0, I(ora), I(asl), 0,  // 18
    I(jsr), I(AND),      0, 0, I(bit), I(AND), I(rol), 0,  // 20
    I(plp), I(AND), I(rol), 0, I(bit), I(AND), I(rol), 0,  // 28
    I(bCC), I(AND),      0, 0,      0, I(AND), I(rol), 0,  // 30
    I(seF), I(AND),      0, 0,      0, I(AND), I(rol), 0,  // 38
         0, I(eor),      0, 0,      0, I(eor), I(lsr), 0,  // 40
    I(pha), I(eor), I(lsr), 0, I(jmp), I(eor), I(lsr), 0,  // 48
    I(bCC), I(eor),      0, 0,      0, I(eor), I(lsr), 0,  // 50
    I(clF), I(eor),      0, 0,      0, I(eor), I(lsr), 0,  // 58
    I(rts), I(adc),      0, 0,      0, I(adc), I(ror), 0,  // 60
    I(pla), I(adc), I(ror), 0, I(jmp), I(adc), I(ror), 0,  // 68
    I(bCC), I(adc),      0, 0,      0, I(adc), I(ror), 0,  // 70
    I(seF), I(adc),      0, 0,      0, I(adc), I(ror), 0,  // 78
         0, I(sta),      0, 0, I(sty), I(sta), I(stx), 0,  // 80
    I(dey), I(sta), I(txa), 0, I(sty), I(sta), I(stx), 0,  // 88
    I(bCC), I(sta),      0, 0, I(sty), I(sta), I(stx), 0,  // 90
    I(tya), I(sta), I(txs), 0,      0, I(sta),      0, 0,  // 98
    I(ldy), I(lda), I(ldx), 0, I(ldy), I(lda), I(ldx), 0,  // A0
    I(tay), I(lda), I(tax), 0, I(ldy), I(lda), I(ldx), 0,  // A8
    I(bCC), I(lda),      0, 0, I(ldy), I(lda), I(ldx), 0,  // B0
    I(clF), I(lda), I(tsx), 0, I(ldy), I(lda), I(ldx), 0,  // B8
    I(cpy), I(cmp),      0, 0, I(cpy), I(cmp), I(dec), 0,  // C0
    I(iny), I(cmp), I(dex), 0, I(cpy), I(cmp), I(dec), 0,  // C8
    I(bCC), I(cmp),      0, 0,      0, I(cmp), I(dec), 0,  // D0
    I(clF), I(cmp),      0, 0,      0, I(cmp), I(dec), 0,  // D8
    I(cpx), I(sbc),      0, 0, I(cpx), I(sbc), I(inc), 0,  // E0
    I(inx), I(sbc), I(nop), 0, I(cpx), I(sbc), I(inc), 0,  // E8
    I(bCC), I(sbc),      0, 0,      0, I(sbc), I(inc), 0,  // F0
    I(seF), I(sbc),      0, 0,      0, I(sbc), I(inc), 0,  // F8
};

void cpu_6502::sta(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    store_ea(ea, A);
}

void cpu_6502::lda(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    A = load_ea(ea);
    set_NZ(A);
}

void cpu_6502::ora(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    alu_ora(load_ea(ea));
}

void cpu_6502::AND(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    alu_and(load_ea(ea));
}

void cpu_6502::eor(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    alu_eor(load_ea(ea));
}

void cpu_6502::adc(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    alu_adc(load_ea(ea));
}

void cpu_6502::sbc(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    alu_sbc(load_ea(ea));
}

void cpu_6502::cmp(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    alu_cmp(A, load_ea(ea));
}

void cpu_6502::stx(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == ABX)
	addrmode = ABY;
    else if (addrmode == ZPX)
	addrmode = ZPY;
    store_ea(get_ea(addrmode), X);
}

void cpu_6502::ldx(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == INX)
	addrmode = IMM;
    else if (addrmode == ABX)
	addrmode = ABY;
    else if (addrmode == ZPX)
	addrmode = ZPY;
    X = load_ea(get_ea(addrmode));
    set_NZ(X);
}

void cpu_6502::cpx(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == INX)
	addrmode = IMM;
    else if (addrmode == ABX)
	addrmode = ABY;
    else if (addrmode == ZPX)
	addrmode = ZPY;
    alu_cmp(X, load_ea(get_ea(addrmode)));
}

void cpu_6502::sty(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    store_ea(get_ea(addrmode), Y);
}

void cpu_6502::ldy(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == INX)
	addrmode = IMM;
    Y = load_ea(get_ea(addrmode));
    set_NZ(Y);
}

void cpu_6502::cpy(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == INX)
	addrmode = IMM;
    alu_cmp(Y, load_ea(get_ea(addrmode)));
}

void cpu_6502::pha(uint8_t opcode)
{
    push(A);
}

void cpu_6502::pla(uint8_t opcode)
{
    A = pull();
    set_NZ(A);
}

void cpu_6502::php(uint8_t opcode)
{
    push(P | 0x30);
}

void cpu_6502::plp(uint8_t opcode)
{
    P = pull() & ~0x30;
}

void cpu_6502::tax(uint8_t opcode)
{
    set_NZ(X = A);
}

void cpu_6502::txa(uint8_t opcode)
{
    set_NZ(A = X);
}

void cpu_6502::tay(uint8_t opcode)
{
    set_NZ(Y = A);
}

void cpu_6502::tya(uint8_t opcode)
{
    set_NZ(A = Y);
}

void cpu_6502::txs(uint8_t opcode)
{
    set_NZ(S = X);
}

void cpu_6502::tsx(uint8_t opcode)
{
    set_NZ(X = S);
}

void cpu_6502::inx(uint8_t opcode)
{
    set_NZ(++X);
}

void cpu_6502::iny(uint8_t opcode)
{
    set_NZ(++Y);
}

void cpu_6502::dex(uint8_t opcode)
{
    set_NZ(--X);
}

void cpu_6502::dey(uint8_t opcode)
{
    set_NZ(--Y);
}

void cpu_6502::inc(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    store_ea(ea, alu_inc(load_ea(ea)));
}

void cpu_6502::dec(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    store_ea(ea, alu_dec(load_ea(ea)));
}

void cpu_6502::asl(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == IMM)
	A = alu_asl(A);
    else {
	struct ea ea = get_ea(addrmode);
	store_ea(ea, alu_asl(load_ea(ea)));
    }
}

void cpu_6502::lsr(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == IMM)
	A = alu_lsr(A);
    else {
	struct ea ea = get_ea(addrmode);
	store_ea(ea, alu_lsr(load_ea(ea)));
    }
}

void cpu_6502::rol(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == IMM)
	A = alu_rol(A);
    else {
	struct ea ea = get_ea(addrmode);
	store_ea(ea, alu_rol(load_ea(ea)));
    }
}

void cpu_6502::ror(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    if (addrmode == IMM)
	A = alu_ror(A);
    else {
	struct ea ea = get_ea(addrmode);
	store_ea(ea, alu_ror(load_ea(ea)));
    }
}

void cpu_6502::bit(uint8_t opcode)
{
    struct ea ea = get_ea((opcode >> 2) & 7);
    uint8_t val = load_ea(ea);
    P = P & ~0xc0 | val & 0xc0;
    Z = (A & val) == 0;
}

// Instructions clc, cli, clv, cld.
void cpu_6502::clF(uint8_t opcode)
{
    // The bit number in register P to be cleared.
    static const char bittab[] = { 0, 2, 6, 3 };

    P &= ~(1 << bittab[opcode >> 6]);
}

// Instructions sec, sei, sed.
// The is no sev instruction, since tya is in its place.
void cpu_6502::seF(uint8_t opcode)
{
    // The bit number in register P to be set.
    static const char bittab[] = { 0, -1, 6, 3 };

    P |= 1 << bittab[opcode >> 6];
}

// Instructions bpl, bmi, bvc, bvs, bcc, bcs, bne, beq.
void cpu_6502::bCC(uint8_t opcode)
{
    // The bit number in register P to be tested.
    static const char bittab[] = { 7, 6, 0, 1 };

    int8_t offset = fetch();
    bool jmp;

    jmp = !!(P & (1 << bittab[opcode >> 6])) ^ !(opcode & 0x20);
    if (jmp)
	PC += offset;
}

void cpu_6502::jmp(uint8_t opcode)
{
    struct ea ea = opcode == 0x4c ? get_ea(ABS) : get_ea(IND);
    PC = ea.addr;
}

void cpu_6502::jsr(uint8_t opcode)
{
    struct ea ea = get_ea(ABS);
    push(PC - 1 >> 8);
    push(PC - 1 & 0xff);
    PC = ea.addr;
}

void cpu_6502::rts(uint8_t opcode)
{
    uint8_t lo = pull();
    uint8_t hi = pull();
    PC = (hi << 8) | lo;
    PC++;
}

void cpu_6502::brk(uint8_t opcode)
{
    push(PC >> 8);
    push(PC & 0xff);
    push(P);
    I = 1;
    uint8_t lo = mem->load(0xfffe);
    uint8_t hi = mem->load(0xffff);
    PC = (hi << 8) | lo;
}

void cpu_6502::rti(uint8_t opcode)
{
    P = pull();
    uint8_t lo = pull();
    uint8_t hi = pull();
    PC = (hi << 8) | lo;
}

void cpu_6502::nop(uint8_t opcode)
{
    return;
}

// push, pull, and effective address calculation.

uint8_t cpu_6502::fetch()
{
    return mem->load(PC++);
}

void cpu_6502::push(uint8_t val)
{
    mem->store(0x100 + S--, val);
}

uint8_t cpu_6502::pull()
{
    return mem->load(0x100 + ++S);
}

struct cpu_6502::ea cpu_6502::get_ea(uint8_t addrmode)
{
    struct ea ea;
    uint8_t  lo, hi;
    uint16_t addr;

    switch (addrmode) {
    case IMM:
	ea.type = ea::IMM;
	ea.val  = fetch();
	return ea;
    case ABS:
	lo = fetch();
	hi = fetch();
	addr = (hi << 8) | lo;
	break;
    case ABX:
	lo = fetch();
	hi = fetch();
	addr = ((hi << 8) | lo) + X;
	break;
    case ABY:
	lo = fetch();
	hi = fetch();
	addr = ((hi << 8) | lo) + Y;
	break;
    case ZP:
	lo = fetch();
	addr = lo;
	break;
    case ZPX:
	// Strange 6502 feature: the index cannot cross
	// from zero page to stack page.
	lo = fetch();
	lo += X;
	addr = lo;
	break;
    case ZPY:
	// Similar to ZPX, no page crossing.
	lo = fetch();
	lo += Y;
	addr = lo;
	break;
    case IND:
	lo = fetch();
	hi = fetch();
	addr = (hi << 8) | lo;
	lo = mem->load(addr);
	// This is a bug in the 6502.
	addr = addr & 0xff00 | (addr + 1 & 0xff);
	hi = mem->load(addr);
	addr = (hi << 8) | lo;
	break;
    case INX:
	addr = fetch();
	addr += X;
	lo = mem->load(addr++);
	hi = mem->load(addr);
	addr = (hi << 8) | lo;
	break;
    case INY:
	addr = fetch();
	lo = mem->load(addr);
	hi = mem->load(addr + 1);
	addr = ((hi << 8) | lo) + Y;
	break;
    default:
	// unreachable, but prevent compiler warning
	addr = 0;
    }
    ea.type = ea::MEM;
    ea.addr = addr;
    return ea;
}

uint8_t cpu_6502::load_ea(struct ea ea)
{
    switch (ea.type) {
    case ea::IMM:
	return ea.val;
    case ea::MEM:
	return mem->load(ea.addr);
    default:
	// unreachable
	return 0;
    };
}

void cpu_6502::store_ea(struct ea ea, uint8_t val)
{
    mem->store(ea.addr, val);
}

// ALU operations

void cpu_6502::alu_adc(uint8_t val)
{
    if (D) {
	// not yet implemented
    } else {
	uint16_t res = A + val + C;
	C = (res & 0x100) != 0;
	V = ((res ^ A) & (res ^ val) & 0x80) != 0;
	set_NZ(A = res);
    }
}

void cpu_6502::alu_sbc(uint8_t val)
{
    if (D) {
	// not yet implemented
    } else
	alu_adc(~val);
}

void cpu_6502::alu_and(uint8_t val)
{
    set_NZ(A &= val);
}

void cpu_6502::alu_ora(uint8_t val)
{
    set_NZ(A |= val);
}

void cpu_6502::alu_eor(uint8_t val)
{
    set_NZ(A ^= val);
}

void cpu_6502::alu_cmp(uint8_t val1, uint8_t val2)
{
    uint16_t res = val1 + (val2 ^ 0xff) + 1;
    // The cmp, cpx, and cpy instructions don't affect the V flag.
    C = (res & 0x100) != 0;
    set_NZ(res);
}

uint8_t cpu_6502::alu_asl(uint8_t val)
{
    C = (val & 0x80) != 0;
    val <<= 1;
    set_NZ(val);
    return val;
}

uint8_t cpu_6502::alu_lsr(uint8_t val)
{
    C = (val & 1) != 0;
    val >>= 1;
    set_NZ(val);
    return val;
}

uint8_t cpu_6502::alu_rol(uint8_t val)
{
    uint8_t cout = (val & 0x80) != 0;
    val <<= 1;
    val |= (C != 0);
    set_NZ(val);
    C = cout;
    return val;
}

uint8_t cpu_6502::alu_ror(uint8_t val)
{
    uint8_t cout = (val & 1) != 0;
    val >>= 1;
    val |= C ? 0x80 : 0;
    set_NZ(val);
    C = cout;
    return val;
}

uint8_t cpu_6502::alu_inc(uint8_t val)
{
    set_NZ(++val);
    return val;
}

uint8_t cpu_6502::alu_dec(uint8_t val)
{
    set_NZ(--val);
    return val;
}

void cpu_6502::set_NZ(uint8_t val)
{
    N = (val & 0x80) != 0;
    Z = (val == 0);
}
