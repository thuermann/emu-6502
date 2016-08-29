//
// $Id: emu-6502.cc,v 1.2 2016/08/29 20:20:24 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>

#include "cpu6502.hh"
#include "memory.hh"

int main(int argc, char **argv)
{
    uint16_t addr  = 0x4000;
    uint16_t reset = 0xfffc;
    cpu_6502 cpu;
    memory   mem;

    int c, offset = 0;
    while ((c = std::cin.get()) != EOF)
	mem.store(addr + offset++, c);
    mem.store(reset,     addr & 0xff);
    mem.store(reset + 1, addr >> 8);

    cpu.attach(&mem);
    cpu.reset();

    return 0;
}
