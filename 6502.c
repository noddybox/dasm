/*

    dasm - Simple, portable disassembler

    Copyright (C) 2025  Ian Cowburn (ianc@noddybox.co.uk)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    -------------------------------------------------------------------------

    6502 disassembly

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "6502.h"
#include "output.h"
#include "input.h"
#include "memory.h"

typedef enum
{
    eImplicit,
    eByte,
    eWord,
    eRelative
} argument_t;

typedef struct
{
    const char  *text;
    argument_t  argtype;
} opcode_t;

static opcode_t optable[256] =
{
};

word C6502_Disassemble(FILE *input, word address)
{
    memory_t mem = INIT_MEMORY;
    word start_address;
    byte opcode;
    opcode_t *op;
    word argument;

    start_address = address;

    opcode = GetByte(input, &address, &mem);

    if (feof(input))
    {
        return start_address;
    }

    op = optable + opcode;

    switch(op->argtype)
    {
        case eImplicit:
            Output(start_address, 4, &mem, op->text);
            break;

        case eByte:
            argument = GetByte(input, &address, &mem);
            Output(start_address, 4, &mem, op->text, argument);
            break;

        case eWord:
            argument = GetLSBWord(input, &address, &mem);
            Output(start_address, 4, &mem, op->text, argument);
            break;

        case eRelative:
            argument = GetRelativeAddress(input, &address, &mem);
            Output(start_address, 4, &mem, op->text, argument);
            break;
    }

    return address;
}

/*
vim: ai sw=4 ts=8 expandtab
*/
