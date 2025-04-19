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

    Input routines.

*/
#include "input.h"

byte GetByte(FILE *fp, word *address, memory_t *memory)
{
    byte b;

    (*address)++;

    b = (byte)getc(fp);
    MemoryAddByte(memory, b);
    return b;
}

int GetRelative(FILE *fp, word *address, memory_t *memory)
{
    relative b;

    b = (relative)GetByte(fp, address, memory);
    return (int)b;
}

word GetRelativeAddress(FILE *fp, word *address, memory_t *memory)
{
    relative offset;
    word result;

    offset = (relative)GetByte(fp, address, memory);
    result = *address + offset;

    return result;
}

word GetLSBWord(FILE *fp, word *address, memory_t *memory)
{
    word hi, lo;

    lo = (word)GetByte(fp, address, memory);
    hi = (word)GetByte(fp, address, memory);

    return lo | hi << 8;
}

word GetMSBWord(FILE *fp, word *address, memory_t *memory)
{
    word hi, lo;

    hi = (word)GetByte(fp, address, memory);
    lo = (word)GetByte(fp, address, memory);

    return lo | hi << 8;
}

/*
vim: ai sw=4 ts=8 expandtab
*/
