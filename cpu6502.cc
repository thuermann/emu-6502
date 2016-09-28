//
// $Id: cpu6502.cc,v 1.21 2016/09/28 00:07:58 urs Exp $
//

#include <cstdint>

#include "cpu6502.hh"

uint8_t cpu_6502::mem_interface::load(uint16_t addr)
{
    uint8_t val = mem->load(addr);
    cpu->observe_mem_access(addr, R, val);
    return val;
}

void cpu_6502::mem_interface::store(uint16_t addr, uint8_t val)
{
    cpu->observe_mem_access(addr, W, val);
    mem->store(addr, val);
}

void cpu_6502::reset()
{
    uint8_t lo = mem.load(RESET);
    uint8_t hi = mem.load(RESET + 1);
    PC = ADDR(lo, hi);
    run();
}

void cpu_6502::run()
{
    observe_init();
    while (1) {
	observe_begin();
	uint8_t opcode = fetch();
	if (instruction ins = itab[opcode])
	    (this->*ins)(opcode);
	observe_end();
	// Temporary hack to provide a way to terminate a program
	if (opcode == 0xff)
	    break;
    }
    observe_finish();
}

#define I(mnc) &cpu_6502::mnc

const cpu_6502::instruction cpu_6502::itab[256] = {
    I(brk), I(ora),      0, 0,      0, I(ora), I(asl), 0,  // 00
    I(php), I(ora), I(asl), 0,      0, I(ora), I(asl), 0,  // 08
    I(bpl), I(ora),      0, 0,      0, I(ora), I(asl), 0,  // 10
    I(clc), I(ora),      0, 0,      0, I(ora), I(asl), 0,  // 18
    I(jsr), I(AND),      0, 0, I(bit), I(AND), I(rol), 0,  // 20
    I(plp), I(AND), I(rol), 0, I(bit), I(AND), I(rol), 0,  // 28
    I(bmi), I(AND),      0, 0,      0, I(AND), I(rol), 0,  // 30
    I(sec), I(AND),      0, 0,      0, I(AND), I(rol), 0,  // 38
    I(rti), I(eor),      0, 0,      0, I(eor), I(lsr), 0,  // 40
    I(pha), I(eor), I(lsr), 0, I(jmp), I(eor), I(lsr), 0,  // 48
    I(bvc), I(eor),      0, 0,      0, I(eor), I(lsr), 0,  // 50
    I(cli), I(eor),      0, 0,      0, I(eor), I(lsr), 0,  // 58
    I(rts), I(adc),      0, 0,      0, I(adc), I(ror), 0,  // 60
    I(pla), I(adc), I(ror), 0, I(jmp), I(adc), I(ror), 0,  // 68
    I(bvs), I(adc),      0, 0,      0, I(adc), I(ror), 0,  // 70
    I(sei), I(adc),      0, 0,      0, I(adc), I(ror), 0,  // 78
         0, I(sta),      0, 0, I(sty), I(sta), I(stx), 0,  // 80
    I(dey), I(sta), I(txa), 0, I(sty), I(sta), I(stx), 0,  // 88
    I(bcc), I(sta),      0, 0, I(sty), I(sta), I(stx), 0,  // 90
    I(tya), I(sta), I(txs), 0,      0, I(sta),      0, 0,  // 98
    I(ldy), I(lda), I(ldx), 0, I(ldy), I(lda), I(ldx), 0,  // A0
    I(tay), I(lda), I(tax), 0, I(ldy), I(lda), I(ldx), 0,  // A8
    I(bcs), I(lda),      0, 0, I(ldy), I(lda), I(ldx), 0,  // B0
    I(clv), I(lda), I(tsx), 0, I(ldy), I(lda), I(ldx), 0,  // B8
    I(cpy), I(cmp),      0, 0, I(cpy), I(cmp), I(dec), 0,  // C0
    I(iny), I(cmp), I(dex), 0, I(cpy), I(cmp), I(dec), 0,  // C8
    I(bne), I(cmp),      0, 0,      0, I(cmp), I(dec), 0,  // D0
    I(cld), I(cmp),      0, 0,      0, I(cmp), I(dec), 0,  // D8
    I(cpx), I(sbc),      0, 0, I(cpx), I(sbc), I(inc), 0,  // E0
    I(inx), I(sbc), I(nop), 0, I(cpx), I(sbc), I(inc), 0,  // E8
    I(beq), I(sbc),      0, 0,      0, I(sbc), I(inc), 0,  // F0
    I(sed), I(sbc),      0, 0,      0, I(sbc), I(inc), 0,  // F8
};

void cpu_6502::sta(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    store_ea(get_ea(addrmode), A);
}

void cpu_6502::lda(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    A = load_ea(get_ea(addrmode));
    set_NZ(A);
}

void cpu_6502::ora(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    alu_ora(load_ea(get_ea(addrmode)));
}

void cpu_6502::AND(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    alu_and(load_ea(get_ea(addrmode)));
}

void cpu_6502::eor(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    alu_eor(load_ea(get_ea(addrmode)));
}

void cpu_6502::adc(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    alu_adc(load_ea(get_ea(addrmode)));
}

void cpu_6502::sbc(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    alu_sbc(load_ea(get_ea(addrmode)));
}

void cpu_6502::cmp(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    alu_cmp(A, load_ea(get_ea(addrmode)));
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
    S = X;
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
    uint8_t addrmode = (opcode >> 2) & 7;
    struct ea ea = get_ea(addrmode);
    store_ea(ea, alu_inc(load_ea(ea)));
}

void cpu_6502::dec(uint8_t opcode)
{
    uint8_t addrmode = (opcode >> 2) & 7;
    struct ea ea = get_ea(addrmode);
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
    uint8_t addrmode = (opcode >> 2) & 7;
    uint8_t val = load_ea(get_ea(addrmode));
    P = P & ~0xc0 | val & 0xc0;
    set(Z, (A & val) == 0);
}

void cpu_6502::clc(uint8_t opcode)
{
    set(C, 0);
}

void cpu_6502::cli(uint8_t opcode)
{
    set(I, 0);
}

void cpu_6502::clv(uint8_t opcode)
{
    set(V, 0);
}

void cpu_6502::cld(uint8_t opcode)
{
    set(D, 0);
}

void cpu_6502::sec(uint8_t opcode)
{
    set(C, 1);
}

void cpu_6502::sei(uint8_t opcode)
{
    set(I, 1);
}

void cpu_6502::sed(uint8_t opcode)
{
    set(D, 1);
}

void cpu_6502::bpl(uint8_t opcode)
{
    int8_t offset = fetch();
    if (!get(N))
	PC += offset;
}

void cpu_6502::bmi(uint8_t opcode)
{
    int8_t offset = fetch();
    if (get(N))
	PC += offset;
}

void cpu_6502::bvc(uint8_t opcode)
{
    int8_t offset = fetch();
    if (!get(V))
	PC += offset;
}

void cpu_6502::bvs(uint8_t opcode)
{
    int8_t offset = fetch();
    if (get(V))
	PC += offset;
}

void cpu_6502::bcc(uint8_t opcode)
{
    int8_t offset = fetch();
    if (!get(C))
	PC += offset;
}

void cpu_6502::bcs(uint8_t opcode)
{
    int8_t offset = fetch();
    if (get(C))
	PC += offset;
}

void cpu_6502::bne(uint8_t opcode)
{
    int8_t offset = fetch();
    if (!get(Z))
	PC += offset;
}

void cpu_6502::beq(uint8_t opcode)
{
    int8_t offset = fetch();
    if (get(Z))
	PC += offset;
}

void cpu_6502::jmp(uint8_t opcode)
{
    struct ea ea = opcode == 0x4c ? get_ea(ABS) : get_ea(IND);
    PC = ea.addr;
}

// The 6502 doesn't push the address of the next instruction to the stack
// but the address - 1.  Therefore, the rts instruction has to add one
// to the address popped off the stack.
void cpu_6502::jsr(uint8_t opcode)
{
    struct ea ea = get_ea(ABS);
    PC--;
    push(HI(PC));
    push(LO(PC));
    PC = ea.addr;
}

void cpu_6502::rts(uint8_t opcode)
{
    uint8_t lo = pull();
    uint8_t hi = pull();
    PC = ADDR(lo, hi);
    PC++;
}

void cpu_6502::brk(uint8_t opcode)
{
    PC++;
    push(HI(PC));
    push(LO(PC));
    push(P | 0x20);
    set(I, 1);
    uint8_t lo = mem.load(IRQ);
    uint8_t hi = mem.load(IRQ + 1);
    PC = ADDR(lo, hi);
}

void cpu_6502::rti(uint8_t opcode)
{
    P = pull() & ~0x30;
    uint8_t lo = pull();
    uint8_t hi = pull();
    PC = ADDR(lo, hi);
}

void cpu_6502::nop(uint8_t opcode)
{
    return;
}

// push, pull, effective address calculation, and opcode fetch

void cpu_6502::push(uint8_t val)
{
    mem.store(0x100 + S--, val);
}

uint8_t cpu_6502::pull()
{
    return mem.load(0x100 + ++S);
}

struct cpu_6502::ea cpu_6502::get_ea(uint8_t addrmode)
{
    struct ea ea;
    uint8_t  lo, hi, zp_addr;
    uint16_t addr;

    switch (addrmode) {
    case IMM:
	ea.type = ea::IMM;
	ea.val  = fetch();
	return ea;
    case ABS:
	lo = fetch();
	hi = fetch();
	addr = ADDR(lo, hi);
	break;
    case ABX:
	lo = fetch();
	hi = fetch();
	addr = ADDR(lo, hi) + X;
	break;
    case ABY:
	lo = fetch();
	hi = fetch();
	addr = ADDR(lo, hi) + Y;
	break;
    case ZP:
	zp_addr = fetch();
	addr = zp_addr;
	break;
    case ZPX:
	// Strange 6502 feature: the index cannot cross
	// from zero page to stack page.
	zp_addr = fetch();
	zp_addr += X;
	addr = zp_addr;
	break;
    case ZPY:
	// Similar to ZPX, no page crossing.
	zp_addr = fetch();
	zp_addr += Y;
	addr = zp_addr;
	break;
    case IND:
	lo = fetch();
	hi = fetch();
	addr = ADDR(lo, hi);
	// The following behavior is a bug in the 6502:
	// If addr points to the last byte in a page, the CPU doesn't
	// read addr and addr + 1, but addr and addr + 1 - 0x100, i.e.
	// the last byte followed by the first byte in that page.
	// I.e. the increment of addr cannot cross a page boundary,
	// instead the low byte of addr is incremented modulo 256.
	lo = mem.load(addr);
	addr = addr & 0xff00 | (addr + 1 & 0xff);
	hi = mem.load(addr);
	addr = ADDR(lo, hi);
	break;
    case INX:
	zp_addr = fetch();
	zp_addr += X;
	lo = mem.load(zp_addr++);
	hi = mem.load(zp_addr);
	addr = ADDR(lo, hi);
	break;
    case INY:
	zp_addr = fetch();
	lo = mem.load(zp_addr++);
	hi = mem.load(zp_addr);
	addr = ADDR(lo, hi) + Y;
	break;
    default:
	// unreachable, but prevent compiler warning
	addr = 0;
    }
    ea.type = ea::MEM;
    ea.addr = addr;
    return ea;
}

uint8_t cpu_6502::fetch()
{
    uint8_t opcode = mem.load(PC++);
    observe_fetch(opcode);
    return opcode;
}

uint8_t cpu_6502::load_ea(struct ea ea)
{
    switch (ea.type) {
    case ea::IMM:
	return ea.val;
    case ea::MEM:
	return mem.load(ea.addr);
    default:
	// unreachable
	return 0;
    };
}

void cpu_6502::store_ea(struct ea ea, uint8_t val)
{
    mem.store(ea.addr, val);
}

// ALU operations

void cpu_6502::alu_adc(uint8_t val)
{
    if (get(D)) {
	// not yet implemented
    } else {
	uint16_t res = A + val + get(C);
	set(C, res & 0x100);
	set(V, (res ^ A) & (res ^ val) & 0x80);
	set_NZ(A = res);
    }
}

void cpu_6502::alu_sbc(uint8_t val)
{
    if (get(D)) {
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
    set(C, res & 0x100);
    set_NZ(res);
}

uint8_t cpu_6502::alu_asl(uint8_t val)
{
    set(C, val & 0x80);
    val <<= 1;
    set_NZ(val);
    return val;
}

uint8_t cpu_6502::alu_lsr(uint8_t val)
{
    set(C, val & 1);
    val >>= 1;
    set_NZ(val);
    return val;
}

uint8_t cpu_6502::alu_rol(uint8_t val)
{
    bool cout = val & 0x80;
    val <<= 1;
    val |= get(C);
    set_NZ(val);
    set(C, cout);
    return val;
}

uint8_t cpu_6502::alu_ror(uint8_t val)
{
    bool cout = val & 1;
    val >>= 1;
    val |= get(C) ? 0x80 : 0;
    set_NZ(val);
    set(C, cout);
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
    set(N, val & 0x80);
    set(Z, val == 0);
}
