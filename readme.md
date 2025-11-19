# P-8 CPU

P-8 is an 8-bit single-cycle RISC CPU.

## Design

- ROM memory for storing instructions and RAM memory for the program to use ([Harvard Architecture](https://en.wikipedia.org/wiki/Harvard_architecture))
- Each instruction is 20 bits long, with the following structure

```c
[(4b opcode) (4b reg1) (4b reg2) (8b imm/ptr)]
```

- The CPU fetches/decodes at the falling edge and executes (writes to memory) at the rising edge, handling the instruction in one clock cycle

### Instruction Table

| opcode (bin) | mnemonic | description                             |
| ------------ | -------- | --------------------------------------- |
| `0000`       | `HLT`    | Halt CPU clock                          |
| `0001`       | `LIM`    | Load imm into \*reg1                    |
| `0010`       | `LOD`    | Load *ptr into *reg1                    |
| `0011`       | `STR`    | Store *reg2 to address at *reg1         |
| `0100`       | `MOV`    | Move *reg2 value to *reg1 (reg1 = reg2) |
| `0101`       | `JMP`    | Jump to imm                             |
| `0110`       | `JZE`    | Jump to imm if register is zero         |
| `0111`       | `JNZ`    | Jump to imm if register is nonzero      |
| `1000`       | `ADD`    | Add \*reg2 value to \*reg1              |
| `1001`       | `SUB`    | ...                                     |
| `1010`       | `MUL`    | ...                                     |
| `1011`       | `DIV`    | ...                                     |
| `1100`       | `NOT`    | ...                                     |
| `1101`       | `AND`    | ...                                     |
| `1110`       | `OR `    | ...                                     |
| `1111`       | `XOR`    | ...                                     |

## Registers

| address (bin) | mnemonic | description     |
| ------------- | -------- | --------------- |
| `0000`        | `%R0`    | General-purpose |
| `0001`        | `%R1`    | General-purpose |
| `0010`        | `%R2`    | General-purpose |
| `0011`        | `%R3`    | General-purpose |
| `0100`        | `%R4`    | General-purpose |
| `0101`        | `%R5`    | General-purpose |
| `0110`        | `%R6`    | General-purpose |
| `0111`        | `%R7`    | General-purpose |

# Assembler

This repository also contains an assembler that compiles source code into raw bytes (Logisim's `v2.0 raw` file format) that can be executed by the cpu.

There are assembler source examples at `/asm/fixtures` and pre-compiled `.hex` files at `/asm/fixtures/bin`

# `.dig` files

The design is implemented in [Helmut Neemann's Digital Logic Designer](https://github.com/hneemann/Digital)

### Made by Vardan2009
