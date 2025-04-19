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

#ifndef DASM_INPUT_H
#define DASM_INPUT_H

#include <stdio.h>
#include "global.h"
#include "memory.h"

byte GetByte(FILE *fp, word *address, memory_t *memory);
int GetRelative(FILE *fp, word *address, memory_t *memory);
word GetRelativeAddress(FILE *fp, word *address, memory_t *memory);
word GetLSBWord(FILE *fp, word *address, memory_t *memory);
word GetMSBWord(FILE *fp, word *address, memory_t *memory);

#endif

/*
vim: ai sw=4 ts=8 expandtab
*/
