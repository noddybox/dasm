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

    Output routines.

*/

#include <stdio.h>
#include <stdarg.h>

#include "output.h"
#include "memory.h"

static int opt[2];

void Output(word address, int address_length, memory_t *mem, const char *opcode,
            const char *arguments, ...)
{
    va_list va;

    if (opt[eShowAddress])
    {
        printf("%*.*x%*.*s", address_length, address_length, address,
                             8 - address_length, 8 - address_length, "");
    }
    else
    {
        printf("%8.8s", "");
    }

    printf("%-8.8s", opcode);

    va_start(va, arguments);
    vprintf(arguments, va);
    va_end(va);

    if (opt[eShowMemory])
    {
        printf(" ; %s", MemoryToString(mem));
    }

    printf("\n");
}

void OutputOption(output_option option, int setting)
{
    opt[option] = setting;
}

/*
vim: ai sw=4 ts=8 expandtab
*/
