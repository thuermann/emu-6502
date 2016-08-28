#
# $Id: Makefile,v 1.1 2016/08/28 02:01:28 urs Exp $
#

CXX	 = g++ -std=gnu++11
CXXFLAGS = -ggdb -Wall -Wextra -Wno-unused-parameter -Wno-parentheses
LDFLAGS  = -ggdb
AS65	 = xa
RM	 = rm -f

programs = emu-6502

.PHONY: all
all: $(programs)

obj = emu-6502.o cpu6502.o
emu-6502: $(obj)
	$(CXX) $(LDFLAGS) -o $@ $(obj)

test.bin: test.s
	 $(AS65) -o $@ test.s

.PHONY: clean
clean:
	$(RM) $(programs) *.o test.bin core

emu-6502.o: cpu6502.hh memory.hh
cpu6502.o:  cpu6502.hh memory.hh
