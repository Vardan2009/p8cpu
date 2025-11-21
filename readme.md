# P-8 CPU

P-8 is an 8-bit single-cycle RISC CPU.

## Design

The design is implemented in [Helmut Neemann's Digital Logic Designer](https://github.com/hneemann/Digital)

- ROM memory for storing instructions and RAM memory for the program to use ([Harvard Architecture](https://en.wikipedia.org/wiki/Harvard_architecture))
- Each instruction is 20 bits long, with the following structure

```
[(5b opcode) (3b reg1) (3b reg2) (8b imm/ptr) (1b mode)]
```

- The CPU fetches/decodes at the falling edge and executes (writes to memory) at the rising edge, handling the instruction in one clock cycle

### Instruction Table

| opcode (bin) | mnemonic | description                                                             | asm example    |
| ------------ | -------- | ----------------------------------------------------------------------- | -------------- |
| `00000`      | `NOP`    | No operation                                                            | `NOP`          |
| `00001`      | `LDI`    | Load `[imm]` into `[reg1]`                                              | `LDI %R0, 123` |
| `00010`      | `LD`     | Load `[ptr OR reg2]` into `[reg1]`                                      | `LD %R0, 123`  |
| `00011`      | `MOV`    | Move `[reg2]` to `[reg1]`                                               | `MOV %R3, %R2` |
| `00100`      | `STI`    | Store `[imm]` to `[reg1 ptr]`                                           | `STI %R5, 123` |
| `00101`      | `ST`     | Store `[reg2]` to `[reg1 ptr]`                                          | `ST %R5, %R1`  |
| `00110`      | `JMP`    | Jump to `[imm OR reg1]`                                                 | `JMP 123`      |
| `00111`      | `JZ`     | Jump to `[imm OR reg1]` if zero flag set                                | `JZ 123`       |
| `01000`      | `JC`     | Jump to `[imm OR reg1]` if carry flag set                               | `...`          |
| `01001`      | `JO`     | Jump to `[imm OR reg1]` if overflow flag set                            | `...`          |
| `01010`      | `JN`     | Jump to `[imm OR reg1]` if negative flag set                            | `...`          |
| `01011`      | `JV`     | Jump to `[imm OR reg1]` if even flag set                                | `...`          |
| `01100`      | `IN`     | Read byte into `[reg1]` from port `[imm OR reg2]`                       | `IN %R5, 123`  |
| `01101`      | `OUT`    | Write byte with `[reg1]` into port `[imm OR reg2]`                      | `OUT %R5, 123` |
| `01110`      | `...`    | Reserved for future use                                                 | `...`          |
| `01111`      | `...`    | Reserved for future use                                                 | `...`          |
| `10000`      | `ADD`    | Add `[imm OR reg2]` to `[reg1]`                                         | `ADD %R1, %R2` |
| `10001`      | `SUB`    | ...                                                                     | `...`          |
| `10010`      | `MUL`    | ...                                                                     | `...`          |
| `10011`      | `DIV`    | ...                                                                     | `...`          |
| `10100`      | `NOT`    | ...                                                                     | `...`          |
| `10101`      | `AND`    | ...                                                                     | `...`          |
| `10110`      | `OR`     | ...                                                                     | `...`          |
| `10111`      | `XOR`    | ...                                                                     | `...`          |
| `11000`      | `SHL`    | Shift `[reg1]` left `[imm OR reg2]`                                     | `...`          |
| `11001`      | `SHR`    | Shift `[reg1]` right `[imm OR reg2]`                                    | `...`          |
| `11010`      | `INC`    | Increment `[reg1]`                                                      | `...`          |
| `11011`      | `DEC`    | Decrement `[reg1]`                                                      | `...`          |
| `11100`      | `TEST`   | Subtracts `[imm OR reg2]` from `[reg1]`, only updates `%FLAGS`          | `...`          |
| `11101`      | `CMP`    | Bitwise AND between `[reg1]` and `[imm OR reg2]`, only updates `%FLAGS` | `...`          |
| `11110`      | `...`    | Reserved for future use                                                 | `...`          |
| `11111`      | `HLT`    | Halt CPU Clock                                                          | `HLT`          |

## Registers

| address (bin) | mnemonic | description     |
| ------------- | -------- | --------------- |
| `000`         | `%R0`    | General-purpose |
| `001`         | `%R1`    | General-purpose |
| `010`         | `%R2`    | General-purpose |
| `011`         | `%R3`    | General-purpose |
| `100`         | `%R4`    | General-purpose |
| `101`         | `%R5`    | General-purpose |
| `110`         | `%SP`    | Stack Pointer   |
| `111`         | `%FLAGS` | ALU Flags       |

## `%FLAGS` bits, in order from MSB to LSB

```
Zero, Carry, Overflow, Negative, Even, (unused), (unused), (unused)
```

# Assembler

This repository also contains an assembler that compiles source code into raw bytes (Logisim's `v2.0 raw` file format) that can be executed by the cpu.

There are assembler source examples at `/asm/fixtures` and pre-compiled `.hex` files at `/asm/fixtures/bin`

---

### Made by Vardan2009
