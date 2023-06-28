#include <6502.h>
#include <stdio.h>
#include <string.h>

cpu6502_t cpu;

void logCallback(int type, const char *msg, uint8_t opcode) // callback called by emulator
{
    switch (type)
    {
    case LIB6502_LOG_EXCEPTION:
        printf("exception: %s -> 0x%x\n", msg, opcode);
        break;
    case LIB6502_LOG_DEBUG:
        printf("debug: %s0x%x\n", msg, opcode);
        break;
    default:
        break;
    }
}

int main()
{
    printf("6502 emulator tester\n");

    memset(cpu.memory, 0xEA, 65535); // set all bytes to 0xEA
    cpu.log = logCallback;

    uint16_t programStart = 0;
    cpu.memory[0xFFFC] = (uint8_t)programStart;
    cpu.memory[0xFFFD] = (uint8_t)(programStart >> 8);

    cpu.memory[0xFF00] = 0x1F;

    #define append(x) cpu.memory[programStart++] = x;

    append(0xA9); // LDA #255
    append(0xFF);

    append(0xAD); // LDA $255
    append(0x00);
    append(0xFF);

    lib6502Init(&cpu); // initialise cpu state

    for (int i = 0; i < 5; i++) // execute 5 instructions
    {
        lib6502Cycle(&cpu);
        printf("%lu: a: %d, y: %d, x: %d, pc: 0x%x, sp: 0x%x, status: 0x%x\n", cpu.instructions, cpu.a, cpu.y, cpu.x, cpu.pc, cpu.sp, cpu.status);
    }
}