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

	//TODO: break these out into their own functions or at least group them. 
	//Maybe redesign the registers interface so that they're more easily addressable

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
					if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]))
					    	V[0xF] = 1; //borrow
						else
					    	V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x0006:
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
					pc += 2;
					break;
				case 0x0007:
					if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
						V[0xF] = 1; //borrow
					else
						V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				case 0x000E:
					V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
					pc += 2;
					break;

			}
		case 0x9000: //9XY0
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 8])
				pc += 4;
			else
				pc += 2;
			break;
		//TODO: more opcodes
		case 0xA000: //ANNN: sets I to address NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000:
			pc = (opcode & 0x0FFF) + V[0x0];
			break;
		case 0xC000:
			//TODO: Sets VX to the result of a bitwise and operation on a random number and NN.
			break;
		case 0xD000:
			/*	
				TODO: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
				Each row of 8 pixels is read as bit-coded starting from memory location I; 
				I value doesn’t change after the execution of this instruction. As described above, 
				VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
				and to 0 if that doesn’t happen
			*/
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;
			
			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(screen[(x + xline + ((y + yline) * 64))] == 1)
							V[0xF] = 1;                                 
						screen[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
			 
			drawFlag = true;
			pc += 2;
			break;
		//TODO: more opcodes
		default:
			printf("Unknown opcode 0x%X\n", opcode);
	}

	// Update timers
	if(delay > 0)
		--delay;
 
	if(sound > 0)
	{
		if(sound == 1)
			printf("BEEP!\n");

		--sound_timer;
	}  
}
