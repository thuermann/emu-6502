#
# $Id: Makefile,v 1.6 2016/09/23 21:55:05 urs Exp $
#

CXX	  = g++ -std=gnu++11
CXXFLAGS  = -ggdb -Wall -Wextra -Wno-unused-parameter -Wno-parentheses
LDFLAGS   = -ggdb
AS65	  = cl65
AS65FLAGS =
RM	  = rm -f

programs  = emu-6502 test.bin count.bin count-down.bin arith.bin demo.bin

.PHONY: all
all: $(programs)

obj = emu-6502.o cpu6502.o observer.o
emu-6502: $(obj)
	$(CXX) $(LDFLAGS) -o $@ $(obj)

.SUFFIXES: .bin
.s.bin:
	 $(AS65) $(AS65FLAGS) -t none -o $@ $<

.PHONY: clean
clean:
	$(RM) $(programs) *.o core

emu-6502.o: cpu6502.hh memory.hh
cpu6502.o:  cpu6502.hh memory.hh
