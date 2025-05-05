# dasm

8 and 16 bit disassembler.

## Usage

Pass the CPU type and file to disassemble and optional arguments.

`dasm -c cpu_type [-o origin] [-a] [-m]  binary_file`

-c chooses the CPU

-o sets the start address of the binary file

-a disables the output of address in the output

-m disables the output of the memory bytes in the output

## Processors

Currently **dasm** supports:

* Z80
