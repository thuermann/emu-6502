//
// $Id: emu-6502.cc,v 1.5 2016/09/23 21:54:55 urs Exp $
//

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "cpu6502.hh"
#include "memory.hh"

static void usage(const char *name)
{
    std::cerr << "Usage: " << name << " [-vV] img-file" << std::endl;
}

int main(int argc, char **argv)
{
    int verbose = 0;
    cpu_6502 cpu;
    memory   mem;

    char *name = argv[0];

    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
	argc--, argv++;
	verbose = 1;
    } else if (argc > 1 && strcmp(argv[1], "-V") == 0) {
	argc--, argv++;
	verbose = 2;
    }

    if (argc != 2) {
	usage(name);
	exit(1);
    }

    // Open the file and get its size.
    const char *fname = argv[1];
    std::ifstream file(fname, std::ifstream::binary);
    if (!file) {
	std::cerr << "Couldn't open file " << fname << '.' << std::endl;
	exit(1);
    }
    file.seekg(0, file.end);
    std::streampos fsize = file.tellg();
    file.seekg(0);

    // Load the file to the end of the 64K memory region and close it.
    int c;
    uint16_t addr = 0x10000 - fsize;
    while ((c = file.get()) != EOF)
	mem.store(addr++, c);

    file.close();

    // Set verbosity of observer, attach the memory to the CPU
    // and run our program.
    cpu.set_verbose(verbose);
    cpu.attach(&mem);
    cpu.reset();

    return 0;
}
