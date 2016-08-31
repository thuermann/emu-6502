#
# $Id: Makefile,v 1.3 2016/08/31 06:06:05 urs Exp $
#

CXX	 = g++ -std=gnu++11
CXXFLAGS = -ggdb -Wall -Wextra -Wno-unused-parameter -Wno-parentheses
LDFLAGS  = -ggdb
AS65	 = xa
RM	 = rm -f

programs = emu-6502 test.bin count.bin count-down.bin arith.bin

.PHONY: all
all: $(programs)

obj = emu-6502.o cpu6502.o
emu-6502: $(obj)
	$(CXX) $(LDFLAGS) -o $@ $(obj)

.SUFFIXES: .bin
.s.bin:
	 $(AS65) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(programs) *.o core

emu-6502.o: cpu6502.hh memory.hh
cpu6502.o:  cpu6502.hh memory.hh
