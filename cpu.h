#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include <stdio.h>
#include <cstring>

class Cpu {
public:
	void init();
	void cycle();
	int loadProgram();

protected:

private:
	uint8_t V[16];
	uint8_t delay;
	uint8_t sound;
	uint16_t opcode; //current opcode
	uint16_t I;
	uint16_t pc; //program counter (stores current address)
	uint8_t sp; //stack pointer (stores current position in stack)
	uint16_t stack[16];
	uint8_t memory[4096];
	uint8_t screen[64][32];
};

#endif