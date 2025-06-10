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
    eImplied,
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
    {/* 00 */   "brk",                  eImplied},
    {/* 01 */   "ora ($%2.2x,x)",       eByte},
    {/* 02 */   "jam",                  eImplied},
    {/* 03 */   "slo ($%2.2x,x)",       eByte},
    {/* 04 */   "nop $%2.2x",           eByte},
    {/* 05 */   "ora $%2.2x",           eByte},
    {/* 06 */   "asl $%2.2x",           eByte},
    {/* 07 */   "slo $%2.2x",           eByte},
    {/* 08 */   "php",                  eImplied},
    {/* 09 */   "ora #$%2.2x",          eByte},
    {/* 0a */   "asl a",                eImplied},
    {/* 0b */   "anc #$%2.2x",          eByte},
    {/* 0c */   "nop $%4.4x",           eWord},
    {/* 0d */   "ora $%4.4x",           eWord},
    {/* 0e */   "asl $%4.4x",           eWord},
    {/* 0f */   "slo $%4.4x",           eWord},

    {/* 10 */   "bpl $%4.4x",           eRelative},
    {/* 11 */   "ora ($%2.2x),y",       eByte},
    {/* 12 */   "jam",                  eImplied},
    {/* 13 */   "slo ($%2.2x),y",       eByte},
    {/* 14 */   "nop $%2.2x,x",         eByte},
    {/* 15 */   "ora $%2.2x,x",         eByte},
    {/* 16 */   "asl $%2.2x,x",         eByte},
    {/* 17 */   "slo $%2.2x,x",         eByte},
    {/* 18 */   "clc",                  eImplied},
    {/* 19 */   "ora $%4.4x,y",         eWord},
    {/* 1a */   "nop",                  eImplied},
    {/* 1b */   "slo $%4.4x,y",         eWord},
    {/* 1c */   "nop $%4.4x,x",         eWord},
    {/* 1d */   "ora $%4.4x,x",         eWord},
    {/* 1e */   "asl $%4.4x,x",         eWord},
    {/* 1f */   "slo $%4.4x,x",         eWord},

    {/* 20 */   "jsr $%4.4x",           eWord},
    {/* 21 */   "and ($%2.2x,x)",       eByte},
    {/* 22 */   "jam",                  eImplied},
    {/* 23 */   "rla ($%2.2x,x)",       eByte},
    {/* 24 */   "bit $%2.2x",           eByte},
    {/* 25 */   "and $%2.2x",           eByte},
    {/* 26 */   "rol $%2.2x",           eByte},
    {/* 27 */   "rla $%2.2x",           eByte},
    {/* 28 */   "plp",                  eImplied},
    {/* 29 */   "and #$%2.2x",          eByte},
    {/* 2a */   "rol a",                eImplied},
    {/* 2b */   "anc2 #$%2.2x",         eByte},
    {/* 2c */   "bit $%4.4x",           eWord},
    {/* 2d */   "and $%4.4x",           eWord},
    {/* 2e */   "rol $%4.4x",           eWord},
    {/* 2f */   "rla $%4.4x",           eWord},

    {/* 30 */   "bmi $%4.4x",           eRelative},
    {/* 31 */   "and ($%2.2x),y",       eByte},
    {/* 32 */   "jam",                  eImplied},
    {/* 33 */   "rla ($%2.2x),y",       eByte},
    {/* 34 */   "nop $%2.2x,x",         eByte},
    {/* 35 */   "and $%2.2x,x",         eByte},
    {/* 36 */   "rol $%2.2x,x",         eByte},
    {/* 37 */   "rla $%2.2x,x",         eByte},
    {/* 38 */   "sec",                  eImplied},
    {/* 39 */   "and $%4.4x,y",         eWord},
    {/* 3a */   "nop",                  eImplied},
    {/* 3b */   "rla $%4.4x,y",         eWord},
    {/* 3c */   "nop $%4.4x,x",         eWord},
    {/* 3d */   "and $%4.4x,x",         eWord},
    {/* 3e */   "rol $%4.4x,x",         eWord},
    {/* 3f */   "rla $%4.4x,x",         eWord},

    {/* 40 */   "rti",                  eImplied},
    {/* 41 */   "eor ($%2.2x,x)",       eByte},
    {/* 42 */   "jam",                  eImplied},
    {/* 43 */   "sre ($%2.2x,x)",       eByte},
    {/* 44 */   "nop $%2.2x",           eByte},
    {/* 45 */   "eor $%2.2x",           eByte},
    {/* 46 */   "lsr $%2.2x",           eByte},
    {/* 47 */   "sre $%2.2x",           eByte},
    {/* 48 */   "pha",                  eImplied},
    {/* 49 */   "eor #$%2.2x",          eByte},
    {/* 4a */   "lsr a",                eImplied},
    {/* 4b */   "alr #$%2.2x",          eByte},
    {/* 4c */   "jmp $%4.4x",           eWord},
    {/* 4d */   "eor $%4.4x",           eWord},
    {/* 4e */   "lsr $%4.4x",           eWord},
    {/* 4f */   "sre $%4.4x",           eWord},

    {/* 50 */   "bvc $%4.4x",           eRelative},
    {/* 51 */   "eor ($%2.2x),y",       eByte},
    {/* 52 */   "jam",                  eImplied},
    {/* 53 */   "sre ($%2.2x),y",       eByte},
    {/* 54 */   "nop $%2.2x,x",         eByte},
    {/* 55 */   "eor $%2.2x,x",         eByte},
    {/* 56 */   "lsr $%2.2x,x",         eByte},
    {/* 57 */   "sre $%2.2x,x",         eByte},
    {/* 58 */   "cli",                  eImplied},
    {/* 59 */   "eor $%4.4x,y",         eWord},
    {/* 5a */   "nop",                  eImplied},
    {/* 5b */   "sre $%4.4x,y",         eWord},
    {/* 5c */   "nop $%4.4x,x",         eWord},
    {/* 5d */   "eor $%4.4x,x",         eWord},
    {/* 5e */   "lsr $%4.4x,x",         eWord},
    {/* 5f */   "sre $%4.4x,x",         eWord},

    {/* 60 */   "rts",                  eImplied},
    {/* 61 */   "adc ($%2.2x,x)",       eByte},
    {/* 62 */   "jam",                  eImplied},
    {/* 63 */   "rra ($%2.2x,x)",       eByte},
    {/* 64 */   "nop $%2.2x",           eByte},
    {/* 65 */   "adc $%2.2x",           eByte},
    {/* 66 */   "ror $%2.2x",           eByte},
    {/* 67 */   "rra $%2.2x",           eByte},
    {/* 68 */   "pla",                  eImplied},
    {/* 69 */   "adc #$%2.2x",          eByte},
    {/* 6a */   "ror a",                eImplied},
    {/* 6b */   "arr #$%2.2x",          eByte},
    {/* 6c */   "jmp ($%4.4x)",         eWord},
    {/* 6d */   "adc $%4.4x",           eWord},
    {/* 6e */   "ror $%4.4x",           eWord},
    {/* 6f */   "rra $%4.4x",           eWord},

    {/* 70 */   "bvs $%4.4x",           eRelative},
    {/* 71 */   "adc ($%2.2x),y",       eByte},
    {/* 72 */   "jam",                  eImplied},
    {/* 73 */   "rra ($%2.2x),y",       eByte},
    {/* 74 */   "nop $%2.2x,x",         eByte},
    {/* 75 */   "adc $%2.2x,x",         eByte},
    {/* 76 */   "ror $%2.2x,x",         eByte},
    {/* 77 */   "rra $%2.2x,x",         eByte},
    {/* 78 */   "sei",                  eImplied},
    {/* 79 */   "adc $%4.4x,y",         eWord},
    {/* 7a */   "nop",                  eImplied},
    {/* 7b */   "rra $%4.4x,y",         eWord},
    {/* 7c */   "nop $%4.4x,x",         eWord},
    {/* 7d */   "adc $%4.4x,x",         eWord},
    {/* 7e */   "ror $%4.4x,x",         eWord},
    {/* 7f */   "rra $%4.4x,x",         eWord},

    {/* 80 */   "nop #$%2.2x",          eByte},
    {/* 81 */   "sta ($%2.2x,x)",       eByte},
    {/* 82 */   "nop #$%2.2x",          eByte},
    {/* 83 */   "sax ($%2.2x,x)",       eByte},
    {/* 84 */   "sty $%2.2x",           eByte},
    {/* 85 */   "sta $%2.2x",           eByte},
    {/* 86 */   "stx $%2.2x",           eByte},
    {/* 87 */   "sax $%2.2x",           eByte},
    {/* 88 */   "dey",                  eImplied},
    {/* 89 */   "nop #$%2.2x",          eByte},
    {/* 8a */   "txa",                  eImplied},
    {/* 8b */   "ane #$%2.2x",          eByte},
    {/* 8c */   "sty $%4.4x",           eWord},
    {/* 8d */   "sta $%4.4x",           eWord},
    {/* 8e */   "stx $%4.4x",           eWord},
    {/* 8f */   "sax $%4.4x",           eWord},

    {/* 90 */   "bcc $%4.4x",           eRelative},
    {/* 91 */   "sta ($%2.2x),y",       eByte},
    {/* 92 */   "jam",                  eImplied},
    {/* 93 */   "sha ($%2.2x),y",       eByte},
    {/* 94 */   "sty $%2.2x,x",         eByte},
    {/* 95 */   "sta $%2.2x,x",         eByte},
    {/* 96 */   "stx $%2.2x,y",         eByte},
    {/* 97 */   "sax $%2.2x,y",         eByte},
    {/* 98 */   "tya",                  eImplied},
    {/* 99 */   "sta $%4.4x,y",         eWord},
    {/* 9a */   "txs",                  eImplied},
    {/* 9b */   "tas $%4.4x,y",         eWord},
    {/* 9c */   "shy $%4.4x,x",         eWord},
    {/* 9d */   "sta $%4.4x,x",         eWord},
    {/* 9e */   "shx $%4.4x,y",         eWord},
    {/* 9f */   "sha $%4.4x,y",         eWord},

    {/* a0 */   "ldy #$%2.2x",          eByte},
    {/* a1 */   "lda ($%2.2x,x)",       eByte},
    {/* a2 */   "ldx #$%2.2x",          eByte},
    {/* a3 */   "lax ($%2.2x,x)",       eByte},
    {/* a4 */   "ldy $%2.2x",           eByte},
    {/* a5 */   "lda $%2.2x",           eByte},
    {/* a6 */   "ldx $%2.2x",           eByte},
    {/* a7 */   "lax $%2.2x",           eByte},
    {/* a8 */   "tay",                  eImplied},
    {/* a9 */   "lda #$%2.2x",          eByte},
    {/* aa */   "tax",                  eImplied},
    {/* ab */   "lxa #$%2.2x",          eByte},
    {/* ac */   "ldy $%4.4x",           eWord},
    {/* ad */   "lda $%4.4x",           eWord},
    {/* ae */   "ldx $%4.4x",           eWord},
    {/* af */   "lax $%4.4x",           eWord},

    {/* b0 */   "bcs $%4.4x",           eRelative},
    {/* b1 */   "lda ($%2.2x),y",       eByte},
    {/* b2 */   "jam",                  eImplied},
    {/* b3 */   "lax ($%2.2x),y",       eByte},
    {/* b4 */   "ldy $%2.2x,x",         eByte},
    {/* b5 */   "lda $%2.2x,x",         eByte},
    {/* b6 */   "ldx $%2.2x,y",         eByte},
    {/* b7 */   "lax $%2.2x,y",         eByte},
    {/* b8 */   "clv",                  eImplied},
    {/* b9 */   "lda $%4.4x,y",         eWord},
    {/* ba */   "tsx",                  eImplied},
    {/* bb */   "las $%4.4x,y",         eWord},
    {/* bc */   "ldy $%4.4x,x",         eWord},
    {/* bd */   "lda $%4.4x,x",         eWord},
    {/* be */   "ldx $%4.4x,y",         eWord},
    {/* bf */   "lax $%4.4x,y",         eWord},

    {/* c0 */   "cpy #$%2.2x",          eByte},
    {/* c1 */   "cmp ($%2.2x,x)",       eByte},
    {/* c2 */   "nop #$%2.2x",          eByte},
    {/* c3 */   "dcp ($%2.2x,x)",       eByte},
    {/* c4 */   "cpy $%2.2x",           eByte},
    {/* c5 */   "cmp $%2.2x",           eByte},
    {/* c6 */   "dec $%2.2x",           eByte},
    {/* c7 */   "dcp $%2.2x",           eByte},
    {/* c8 */   "iny",                  eImplied},
    {/* c9 */   "cmp #$%2.2x",          eByte},
    {/* ca */   "dex",                  eImplied},
    {/* cb */   "sbx #$%2.2x",          eByte},
    {/* cc */   "cpy $%4.4x",           eWord},
    {/* cd */   "cmp $%4.4x",           eWord},
    {/* ce */   "dec $%4.4x",           eWord},
    {/* cf */   "dcp $%4.4x",           eWord},

    {/* d0 */   "bne $%4.4x",           eRelative},
    {/* d1 */   "cmp ($%2.2x),y",       eByte},
    {/* d2 */   "jam",                  eImplied},
    {/* d3 */   "dcp ($%2.2x),y",       eByte},
    {/* d4 */   "nop $%2.2x,x",         eByte},
    {/* d5 */   "cmp $%2.2x,x",         eByte},
    {/* d6 */   "dec $%2.2x,x",         eByte},
    {/* d7 */   "dcp $%2.2x,x",         eByte},
    {/* d8 */   "cld",                  eImplied},
    {/* d9 */   "cmp $%4.4x,y",         eWord},
    {/* da */   "nop",                  eImplied},
    {/* db */   "dcp $%4.4x,y",         eWord},
    {/* dc */   "nop $%4.4x,x",         eWord},
    {/* dd */   "cmp $%4.4x,x",         eWord},
    {/* de */   "dec $%4.4x,x",         eWord},
    {/* df */   "dcp $%4.4x,x",         eWord},

    {/* e0 */   "cpx #$%2.2x",          eByte},
    {/* e1 */   "sbc ($%2.2x,x)",       eByte},
    {/* e2 */   "nop #$%2.2x",          eByte},
    {/* e3 */   "isc ($%2.2x,x)",       eByte},
    {/* e4 */   "cpx $%2.2x",           eByte},
    {/* e5 */   "sbc $%2.2x",           eByte},
    {/* e6 */   "inc $%2.2x",           eByte},
    {/* e7 */   "isc $%2.2x",           eByte},
    {/* e8 */   "inx",                  eImplied},
    {/* e9 */   "sbc #$%2.2x",          eByte},
    {/* ea */   "nop",                  eImplied},
    {/* eb */   "usbc #$%2.2x",         eByte},
    {/* ec */   "cpx $%4.4x",           eWord},
    {/* ed */   "sbc $%4.4x",           eWord},
    {/* ee */   "inc $%4.4x",           eWord},
    {/* ef */   "isc $%4.4x",           eWord},

    {/* f0 */   "beq $%4.4x",           eRelative},
    {/* f1 */   "sbc ($%2.2x),y",       eByte},
    {/* f2 */   "jam",                  eImplied},
    {/* f3 */   "isc ($%2.2x),y",       eByte},
    {/* f4 */   "nop $%2.2x,x",         eByte},
    {/* f5 */   "sbc $%2.2x,x",         eByte},
    {/* f6 */   "inc $%2.2x,x",         eByte},
    {/* f7 */   "isc $%2.2x,x",         eByte},
    {/* f8 */   "sed",                  eImplied},
    {/* f9 */   "sbc $%4.4x,y",         eWord},
    {/* fa */   "nop",                  eImplied},
    {/* fb */   "isc $%4.4x,y",         eWord},
    {/* fc */   "nop $%4.4x,x",         eWord},
    {/* fd */   "sbc $%4.4x,x",         eWord},
    {/* fe */   "inc $%4.4x,x",         eWord},
    {/* ff */   "isc $%4.4x,x",         eWord},
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
        case eImplied:
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
