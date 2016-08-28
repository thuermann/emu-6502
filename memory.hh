//
// $Id: memory.hh,v 1.1 2016/08/28 02:01:28 urs Exp $
//

#ifndef MEMORY_HH
#define MEMORY_HH

#include <cstdint>
#include <cstring>

class memory {
public:
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
