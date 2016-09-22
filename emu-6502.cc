//
// $Id: emu-6502.cc,v 1.4 2016/09/22 23:54:17 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>

#include "cpu6502.hh"
#include "memory.hh"

int main(int argc, char **argv)
{
    int verbose = 0;
    uint16_t addr  = 0x4000;
    uint16_t reset = 0xfffc;
    cpu_6502 cpu;
    memory   mem;

    if (argc > 1 && strcmp(argv[1], "-v") == 0)
	verbose = 1;
    else if (argc > 1 && strcmp(argv[1], "-V") == 0)
	verbose = 2;

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
