//
// $Id: emu-6502.cc,v 1.1 2016/08/28 02:01:28 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>

#include "cpu6502.hh"
#include "memory.hh"

int main(int argc, char **argv)
{
    uint16_t addr = 0x4000;
    cpu_6502 cpu;
    memory   mem;

    int c, offset = 0;
    while ((c = std::cin.get()) != EOF)
	mem.store(addr + offset++, c);

    cpu.attach(&mem);
    cpu.run(addr);

    return 0;
}
