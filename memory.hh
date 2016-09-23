//
// $Id: memory.hh,v 1.2 2016/09/23 21:54:55 urs Exp $
//

#ifndef MEMORY_HH
#define MEMORY_HH

#include <cstdint>
#include <cstring>

class memory {
public:
    // Initialize memory with a pattern that we can easily recognize
    // e.g. in a memory dump, and that is somewhat uncommon, so we can
    // identify memory regions that haven't been touched (written to)
    // by the emulator.
    memory() {
	memset(mem, 0x55, sizeof(mem));
    }
    void store(uint16_t addr, uint8_t val) {
	mem[addr] = val;
    }
    uint8_t load(uint16_t addr) {
	return mem[addr];
    }
private:
    uint8_t mem[65536];
};

#endif
