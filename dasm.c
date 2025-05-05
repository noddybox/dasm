/*

    dasm - Simple, portable assembler

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

    Main

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "output.h"

/* ---------------------------------------- PROCESSORS
*/
#include "z80.h"


/* ---------------------------------------- MACROS
*/


/* ---------------------------------------- VERSION INFO
*/

static const char *dasm_usage =
"Version 1.0\n"
"\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License (Version 3) for more details.\n"
"\n"
"usage: dasm -c cpu [-o address] [-a] [-m] file\n";


/* ---------------------------------------- TYPES
*/

/* Defines a CPU
*/
typedef struct
{
    const char          *name;
    word                (*disassemble)(FILE *input, word address);
} CPU;


/* ---------------------------------------- GLOBALS
*/
static const CPU cpu_table[]=
{
    {
        "Z80",
        Z80_Disassemble,
    },

    {NULL}
};

static const CPU *cpu;


/* ---------------------------------------- UTILS
*/
static int StrEqual(const char *a, const char *b)
{
    while(*a && tolower((unsigned char)*a) == tolower((unsigned char)*b))
    {
        a++;
        b++;
    }

    return tolower((unsigned char)*a) == tolower((unsigned char)*b);
}


/* ---------------------------------------- MAIN
*/
int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    word address = 0;
    int f;
    int n;

    OutputOption(eShowAddress, 1);
    OutputOption(eShowMemory, 1);

    for(f = 1; f < argc && argv[f][0] == '-'; f++)
    {
        switch(argv[f][1])
        {
            case 'c':
                for(n = 0; cpu_table[n].name && !cpu; n++)
                {
                    if (StrEqual(cpu_table[n].name, argv[f+1]))
                    {
                        cpu = cpu_table + n;
                    }
                }
                f++;
                break;

            case 'o':
                address = (word)strtol(argv[++f], NULL, 0);
                break;

            case 'a':
                OutputOption(eShowAddress, 0);
                break;

            case 'm':
                OutputOption(eShowMemory, 0);
                break;

            default:
                break;
        }
    }

    if (f < argc)
    {
        fp = fopen(argv[f], "r");
    }

    if (!fp || !cpu)
    {
        fprintf(stderr,"%s\n", dasm_usage);
        exit(EXIT_FAILURE);
    }

    while(!feof(fp))
    {
        address = cpu->disassemble(fp, address);
    }

    return EXIT_SUCCESS;
}


/*
vim: ai sw=4 ts=8 expandtab
*/
