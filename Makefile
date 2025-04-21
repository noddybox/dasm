# dasm - Simple, portable disassembler
#
# Copyright (C) 2025  Ian Cowburn (ianc@noddybox.co.uk)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# -------------------------------------------------------------------------
#
# Makefile
#
CFLAGS +=	-g

TARGET	=	dasm

SOURCE	=	dasm.c		\
		output.c	\
		input.c		\
		memory.c	\
		z80.c

OBJECTS	=	dasm.o		\
		output.o	\
		input.o		\
		memory.o	\
		z80.o

$(TARGET): $(OBJECTS)
	$(CC) $(CLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(TARGET) $(TARGET).exe $(OBJECTS) core *.core

dasm.o: dasm.c global.h output.h memory.h z80.h
input.o: input.c input.h global.h memory.h
memory.o: memory.c memory.h global.h
output.o: output.c output.h global.h memory.h
z80.o: z80.c z80.h global.h output.h memory.h input.h
