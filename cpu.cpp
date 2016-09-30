#include "cpu.h"

void Cpu::init() {
	pc		= 0x200; 	//program start address is 0x200
	opcode	= 0; 		//reset opcode
	I 		= 0; 		//reset index register
	sp		= 0; 		//reset stack pointer

	//wipe display
	memset(screen, 0x0, sizeof(uint8_t) * sizeof(screen));
	//wipe stack
	memset(stack, 0x0, sizeof(uint16_t) * sizeof(stack));
	//wipe registers
	memset(V, 0x0, sizeof(uint8_t) * sizeof(V));
	//wipe memory
	memset(memory, 0x0, 0x1000);
	//TODO: load fontset
	//TODO: reset timers
}


//Opcode documentation at https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
void Cpu::cycle() {
	//get opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	//TODO: break these out into their own functions or at least group them

	//lop off the trailing 12 bits to narrow down what instruction we're trying to do
	switch(opcode & 0xF000) {
		case 0x0000: //0x0 instructions
			switch (opcode & 0x0FFF) {
			case 0x00E0:
				memset(screen, 0x0, sizeof(uint8_t) * sizeof(screen));
				pc += 2;
				break;
			case 0x00EE:
				pc = opcode & 0x0FFF;
				stack[sp] = 0;
				--sp;
				break;
			}
			break;
		case 0x1000: //jump to address NNN
			pc = opcode & 0x0FFF; //jump to an address.
			break;
		case 0x2000: //call subroutine at NNN
			stack[sp] = pc;
			++sp;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000: // 3XNN: Skips the next instruction if VX equals NN.
			if (V[(opcode & 0x0F00) >> 8] == opcode & 0x00FF) {
				pc += 4;
			} else 
				pc += 2;
			break;
		case 0x4000: //3XNN: Skips the next instruction if VX doesn't equal NN.
			if (V[(opcode & 0x0F00) >> 8] != opcode & 0x00FF)
				pc += 4;
			else 
				pc += 2;
			break;
		case 0x5000: //4XNN: Skips the next instruction if VX equals VY.
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) 
				pc += 4;
			else
				pc += 2;
			break;
		case 0x6000: //6XNN: Sets VX to NN.
			V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			pc += 2;
			break;
		case 0x7000:
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			pc += 2;
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0000:
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x0001:
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x0002:
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x0003:
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x0004:
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				    	V[0xF] = 1; //carry
					else
				    	V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x0005:
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
					    	V[0xF] = 0; //borrow
						else
					    	V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
			}
		//TODO: more opcodes
		case 0xA000: //ANNN: sets I to address NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		//TODO: more opcodes
		default:
			printf("Unknown opcode 0x%X\n", opcode);
	}
}
