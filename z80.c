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

    Z80 implementation

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "z80.h"
#include "output.h"
#include "input.h"
#include "memory.h"

typedef enum
{
    eIX,
    eIY,
    eNone
} IXYShift;

static const char *reg[] =
{
    "b",
    "c",
    "d",
    "e",
    "h",
    "l",
    "(hl)",
    "a"
};

static const char *reg_ix[] =
{
    "b",
    "c",
    "d",
    "e",
    "ixh",
    "ixl",
    "(hl)",     /* Will be replaced by ix+d */
    "a"
};

static const char *reg_iy[] =
{
    "b",
    "c",
    "d",
    "e",
    "iyh",
    "iyl",
    "(hl)",     /* Will be replace by iy+d */
    "a"
};

static const char *reg_pair1[] =
{
    "bc",
    "de",
    "hl",
    "sp"
};

static const char *reg_pair1_ix[] =
{
    "bc",
    "de",
    "ix",
    "sp"
};

static const char *reg_pair1_iy[] =
{
    "bc",
    "de",
    "iy",
    "sp"
};

static const char *reg_pair2[] =
{
    "bc",
    "de",
    "hl",
    "af"
};

static const char *reg_pair2_ix[] =
{
    "bc",
    "de",
    "ix",
    "af"
};

static const char *reg_pair2_iy[] =
{
    "bc",
    "de",
    "iy",
    "af"
};

static const char *cond[] =
{
    "nz",
    "z",
    "nc",
    "c",
    "po",
    "pe",
    "p",
    "m"
};

static const char *alu[][2] =
{
    {"add", "a"},
    {"adc", "a"},
    {"sub", ""},
    {"sbc", "a"},
    {"and", ""},
    {"xor", ""},
    {"or", ""},
    {"cp" ""},
};

static const char *rot[] =
{
    "rlc",
    "rrc",
    "rl",
    "rr",
    "sla",
    "sra",
    "sll",
    "srl"
};

static const char *im[] =
{
    "0",
    "0/1",
    "1",
    "2",
    "0",
    "0/1",
    "1",
    "2"
};

static const char *blit[][4] = 
{
    {"ldi", "cpi", "ini", "outi"},
    {"ldd", "cpd", "ind", "outd"},
    {"ldir", "cpir", "inir", "otir"},
    {"lddr", "cpdr", "indr", "otdr"}
};

static const char *GetIndex(const char *reg, IXYShift ixy_shift,
                            FILE *fp, word *address, memory_t *mem)
{
    static char buff[128];

    switch(ixy_shift)
    {
        case eIX:
            if (strcmp(reg, "(hl)") == 0)
            {
                snprintf(buff, sizeof buff, "(ix%+d)",
                                GetRelative(fp, address, mem));

                return buff;
            }
            else
            {
                return reg;
            }

        case eIY:
            if (strcmp(reg, "(hl)") == 0)
            {
                snprintf(buff, sizeof buff, "(iy%+d)",
                                GetRelative(fp, address, mem));

                return buff;
            }
            else
            {
                return reg;
            }

        default:
            return reg;
    }
}

word Z80_Disassemble(FILE *input, word address)
{
    memory_t mem = INIT_MEMORY;
    word x,y,z,p,q;
    IXYShift ixy_shift = eNone;
    int cb_shift = 0;
    int ed_shift = 0;
    relative offset = 0;
    byte opcode;
    word start_address;

    start_address = address;

get_opcode:
    opcode = GetByte(input, &address, &mem);

    /* Loop through for shifts
    */
    if (opcode == 0xcb)
    {
        cb_shift = 1;
        goto get_opcode;
    }

    if (opcode == 0xed)
    {
        ed_shift = 1;
        goto get_opcode;
    }

    if (opcode == 0xdd)
    {
        ixy_shift = eIX;
        goto get_opcode;
    }

    if (opcode == 0xfd)
    {
        ixy_shift = eIY;
        goto get_opcode;
    }

    /* There is a mandatory displacement before the opcode for DD/FD CB
    */
    if (cb_shift && ixy_shift != eNone)
    {
        offset = (relative)opcode;
        opcode = GetByte(input, &address, &mem);
    }

    /* Decoding info taken from z80.info
    */
    x = (opcode & 0xc0) >> 6;
    y = (opcode & 0x38) >> 3;
    z = (opcode & 0x07);
    p = y >> 1;
    q = y & 1;

    if (!cb_shift && !ed_shift)
    {
        const char **r, **rp, **rp2, *hl;

        switch(ixy_shift)
        {
            case eNone:
                r = reg;
                rp = reg_pair1;
                rp2 = reg_pair2;
                hl = "hl";
                break;
            case eIX:
                r = reg_ix;
                rp = reg_pair1_ix;
                rp2 = reg_pair2_ix;
                hl = "ix";
                break;
            case eIY:
                r = reg_iy;
                rp = reg_pair1_iy;
                rp2 = reg_pair2_iy;
                hl = "iy";
                break;
        }

        if (x == 0)
        {
            if (z == 0)
            {
                switch(y)
                {
                    case 0:
                        Output(start_address, 4, &mem, "nop", "");
                        break;
                    case 1:
                        Output(start_address, 4, &mem, "ex", "af,af'");
                        break;
                    case 2:
                        Output(start_address, 4, &mem, "djnz",
                               "$%4.4x",
                                GetRelativeAddress(input, &address, &mem));
                        break;
                    case 3:
                        Output(start_address, 4, &mem, "jr",
                               "$%4.4x",
                                GetRelativeAddress(input, &address, &mem));
                        break;
                    default:
                        Output(start_address, 4, &mem, "jr",
                               "%s,$%4.4x", cond[y - 4],
                                    GetRelativeAddress(input, &address, &mem));
                        break;
                }
            }

            if (z == 1)
            {
                if (q == 0)
                {
                    Output(start_address, 4, &mem, "ld", "%s,$%4.4x",
                           rp[p], GetLSBWord(input, &address, &mem));
                }

                if (q == 1)
                {
                    Output(start_address, 4, &mem, "add", "%s,%s", hl, rp[p]);
                }
            }

            if (z == 2)
            {
                if (q == 0)
                {
                    switch(p)
                    {
                        case 0:
                            Output(start_address, 4, &mem, "ld", "(bc),a");
                            break;
                        case 1:
                            Output(start_address, 4, &mem, "ld", "(de),a");
                            break;
                        case 2:
                            Output(start_address, 4, &mem, "ld", "($%4.4x),%s",
                                            GetLSBWord(input, &address, &mem),
                                            hl);
                            break;
                        case 3:
                            Output(start_address, 4, &mem, "ld", "($%4.4x),a",
                                            GetLSBWord(input, &address, &mem));
                            break;
                    }
                }

                if (q == 1)
                {
                    switch(p)
                    {
                        case 0:
                            Output(start_address, 4, &mem, "ld", "a,(bc)");
                            break;
                        case 1:
                            Output(start_address, 4, &mem, "ld", "a,(de)");
                            break;
                        case 2:
                            Output(start_address, 4, &mem, "ld", "%s,($%4.4x)",
                                            hl,
                                            GetLSBWord(input, &address, &mem));
                                                
                            break;
                        case 3:
                            Output(start_address, 4, &mem, "ld", "a,($%4.4x)",
                                            GetLSBWord(input, &address, &mem));
                            break;
                    }
                }
            }

            if (z == 3)
            {
                const char *op;

                if (q == 9)
                {
                    op = "inc";
                }
                else
                {
                    op = "dec";
                }

                Output(start_address, 4, &mem, op, "%s", rp[p]);
            }

            if (z == 4)
            {
                Output(start_address, 4, &mem, "inc", "%s",
                        GetIndex(r[y], ixy_shift, input, &address, &mem));
            }

            if (z == 5)
            {
                Output(start_address, 4, &mem, "dec", "%s", 
                        GetIndex(r[y], ixy_shift, input, &address, &mem));
            }

            if (z == 6)
            {
                const char *index =
                        GetIndex(r[y], ixy_shift, input, &address, &mem);

                Output(start_address, 4, &mem, "ld", "%s,$%2.2x",
                        index, GetByte(input, &address, &mem));
            }

            if (z == 7)
            {
                const char *op[] =
                {
                    "rlca", "rrca", "rla", "rra", "daa", "cpl", "scf", "ccf"
                };

                Output(start_address, 4, &mem, op[y], "");
            }
        }
    }

    return address;
}

/*
vim: ai sw=4 ts=8 expandtab
*/
