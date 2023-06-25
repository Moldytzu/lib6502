#pragma once
#include <stdint.h>
#include <stddef.h>

#define LIB6502_LOG_EXCEPTION 1

typedef struct
{
    uint8_t a;      // accumulator A
    uint8_t y;      // index Y
    uint8_t x;      // index X
    uint16_t pc;    // program counter
    uint16_t sp;    // stack pointer
    uint8_t status; // status register

    uint64_t instructions; // total instructions executed

    uint8_t memory[65536]; // 64k addresable memory

    void (*log)(int, const char *, uint8_t); // logging callback
} cpu6502_t;

static void lib6502Init(cpu6502_t *cpu)
{
    cpu->a = cpu->y = cpu->x = 0;
    cpu->pc = (cpu->memory[0xFFFD] << 8) | cpu->memory[0xFFFC]; // program counter is initialised by the reset vector
    cpu->sp = 0x100;                                            // starts at 0x100 and ends at 0x1FF
    cpu->status = 0b00110100;                                   // enables decimal mode and disables interrupts
}

static void lib6502Cycle(cpu6502_t *cpu)
{
    /*  pages 15->19 of W65C02S datasheet
        Addressing modes:
        - Absolute a
            With Absolute addressing the second and third bytes of the instruction form the 16-bit address.
        - Absolute Indexed Indirect (a,x)
            With the Absolute Indexed Indirect addressing mode, the X Index Register is added to the second and third
            bytes of the instruction to form an address to a pointer. This address mode is only used with the JMP
            instruction and the Program Counter is loaded with the first and second bytes at this pointer.
        - Absolute Indexed with X a,x
            With the Absolute Indexed with X addressing mode, the X Index Register is added to the second and third
            bytes of the instruction to form the 16-bits of the effective address.
        - Absolute Indexed with Y a, y
            With the Absolute Indexed with Y addressing mode, the Y Index Register is added to the second and third
            bytes of the instruction to form the 16-bit effective address.
        - Absolute Indirect (a)
            With the Absolute Indirect addressing mode, the second and third bytes of the instruction form an address
            to a pointer. This address mode is only used with the JMP instruction and the Program Counter is loaded
            with the first and second bytes at this pointer
        - Accumulator A
            With Accumulator addressing the operand is implied as the Accumulator and therefore only a single byte
            forms the instruction.
        - Immediate Addressing #
            With Immediate Addressing the operand is the second byte of the instruction.
        - Implied i
            Implied addressing uses a single byte instruction. The operand is implicitly defined by the instruction.
        - Program Counter Relative r
            The Program Counter relative addressing mode, sometimes referred to as Relative Addressing, is used
            with the Branch instructions. If the condition being tested is met, the second byte of the instruction is added
            to the Program Counter and program control is transferred to this new memory location.
        - Stack s
            The Stack may use memory from 0100 to 01FF and the effective address of the Stack address mode will
            always be within this range. Stack addressing refers to all instructions that push or pull data from the stack,
            such as Push, Pull, Jump to Subroutine, Return from Subroutine, Interrupts and Return from Interrupt.
        - Zero Page zp
            With Zero Page (zp) addressing the second byte of the instruction is the address of the operand in page
            zero.
        - Zero Page Indexed Indirect (zp,x)
            The Zero Page Indexed Indirect addressing mode is often referred to as Indirect,X. The second byte of the
            instruction is the zero page address to which the X Index Register is added and the result points to the low
            byte of the indirect address.
        - Zero Page Indexed with X zp,x
            With Zero Page Indexed with X addressing mode, the X Index Register is added to the second byte of
            instruction to form the effective address.
        - Zero Page Indexed with Y zp, y
            With Zero Page Indexed with Y addressing, the second byte of the instruction is the zero page address to
            which the Y Index Register is added to form the page zero effective address.
        - Zero Page Indirect (zp)
            With Zero Page Indirect addressing mode, the second byte of the instruction is a zero page indirect address
            that points to the low byte of a two byte effective address
        - Zero Page Indirect Indexed with Y (zp), y
            The Zero Page Indirect Indexed with Y addressing mode is often referred to as Indirect Y. The second byte
            of the instruction points to the low byte of a two byte (16-bit) base address in page zero. Y Index Register
            is added to the base address to form the effective address.
    */

    uint8_t opcode = cpu->memory[cpu->pc];

    switch (opcode)
    {
    case 0xA9: // LDA #
        cpu->a = cpu->memory[cpu->pc + 1];
        break;
    default:
        cpu->log(LIB6502_LOG_EXCEPTION, "Undefined instruction", opcode);
        break;
    }

    cpu->pc += 3;
    cpu->instructions++;
}