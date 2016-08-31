//
// $Id: emu-6502.cc,v 1.3 2016/08/31 05:34:59 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>

#include "cpu6502.hh"
#include "memory.hh"

int main(int argc, char **argv)
{
    bool verbose = false;
    uint16_t addr  = 0x4000;
    uint16_t reset = 0xfffc;
    cpu_6502 cpu;
    memory   mem;

    if (argc > 1 && strcmp(argv[1], "-v") == 0)
	verbose = true;

    int c, offset = 0;
    while ((c = std::cin.get()) != EOF)
	mem.store(addr + offset++, c);
    mem.store(reset,     addr & 0xff);
    mem.store(reset + 1, addr >> 8);

    cpu.set_verbose(verbose);
    cpu.attach(&mem);
    cpu.reset();

    return 0;
}
