#include <6502.h>
#include <stdio.h>

cpu6502_t cpu;

void logCallback(int type, const char *msg, uint8_t opcode) // callback called by emulator
{
    printf("log (%d): %s -> %x\n", type, msg, opcode);
}

int main()
{
    printf("6502 emulator tester\n");

    cpu.log = logCallback;

    uint16_t programStart = 0x200;
    cpu.memory[0xFFFC] = (uint8_t)programStart;
    cpu.memory[0xFFFD] = (uint8_t)(programStart >> 8);

    cpu.memory[programStart + 0] = 0xA9; // LDA #0xFF
    cpu.memory[programStart + 1] = 0xFF;
    cpu.memory[programStart + 2] = 0x00;

    lib6502Init(&cpu); // initialise cpu state

    for (int i = 0; i < 5; i++) // execute 5 instructions
    {
        lib6502Cycle(&cpu);
        printf("%lu: a: %d, y: %d, x: %d, pc: 0x%x, sp: 0x%x, status: 0x%x\n", cpu.instructions, cpu.a, cpu.y, cpu.x, cpu.pc, cpu.sp, cpu.status);
    }
}