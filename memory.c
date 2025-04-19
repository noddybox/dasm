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

    Memory buffer routines

*/

#include <stdio.h>
#include <string.h>
#include "memory.h"

void MemoryAddByte(memory_t *m, byte b)
{
    if (m->no < MAX_MEMORY_BUFFER)
    {
        m->mem[m->no++] = b;
    }
}

const char *MemoryToString(const memory_t *m)
{
    static char buff[MAX_MEMORY_BUFFER * 5];
    int f;

    buff[0] = 0;

    for(f = 0; f < m->no; f++)
    {
        sprintf(buff + strlen(buff), "%s%2.2x",
                f > 0 ? " " : "", (unsigned)m->mem[f]);
    }

    return buff;
}

/*
vim: ai sw=4 ts=8 expandtab
*/
