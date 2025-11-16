# P-8, 8-bit single-cycle RISC CPU
```
## Design
- ROM for Instruction Memory, RAM for Program Memory

## Instruction Structure
- 20 bits/instruction 
[(4b opcode) (4b reg1) (4b reg2) (8b imm/ptr)]

### Instruction Table

opcode			mnemonic			description
0000            HLT                 Halt CPU clock 
0001            LIM                 Load imm into *reg1
0010            LOD                 Load *ptr into *reg1
0011            SIM                 Store *reg to *ptr1
0100            MOV                 Move *reg2 value to *reg1 (reg1 = reg2)
0101            JMP                 Jump to imm
0110            JZE                 Jump to imm if register is zero
0111            JNZ                 Jump to imm if register is nonzero
1000            ADD                 Add *reg2 value to *reg1
1001            SUB                 ...
1010            MUL                 ...
1011            DIV                 ...
1100            NOT                 ...
1101            AND                 ...
1110            OR                  ...
1111            XOR                 ...

## Registers

address         mnemonic            description
0000            R0                  General-purpose
0001            R1                  General-purpose
0010            R2                  General-purpose
0011            R3                  General-purpose
0100            R4                  General-purpose
0101            R5                  General-purpose
0110            R6                  General-purpose
0111            R7                  General-purpose
```
