//
// $Id: observer.cc,v 1.2 2016/09/22 19:58:42 urs Exp $
//

#include <iostream>
#include <iomanip>
#include <cstdint>

#include "cpu6502.hh"

void cpu_6502::observe_init()
{
    icount = 0;
    std::cout << std::hex << std::setfill('0');
}

void cpu_6502::observe_finish()
{
    observe_prt_cpu_state();
    std::cout << '\n' << std::dec << icount << " instructions executed."
	      << std::endl;
}

void cpu_6502::observe_mem_access(uint16_t addr, enum dir d, uint8_t val)
{
    int i = mlen++;
    maccess[i].addr = addr, maccess[i].dir = d, maccess[i].val = val;
}

void cpu_6502::observe_fetch(uint8_t opcode)
{
    opc[opclen++] = opcode;
}

void cpu_6502::observe_begin()
{
    opclen = mlen = 0;
    if (verbose) {
	observe_prt_cpu_state();
	std::cout << "\texecuting " << std::setw(4) << PC << ": ";
    }
}

void cpu_6502::observe_end()
{
    if (verbose) {
	for (int i = 0; i < 3; i++)
	    if (i < opclen)
		std::cout << ' ' << std::setw(2) << (int)opc[i];
	    else
		std::cout << "   ";
	std::cout << std::endl;
	for (int i = 0; i < mlen; i++)
	    std::cout << "\t\t\t"
		      << std::setw(4) << maccess[i].addr << ' '
		      << maccess[i].dir["RW"] << ' '
		      << std::setw(2) << (int)maccess[i].val << std::endl;
    }
    icount++;
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
