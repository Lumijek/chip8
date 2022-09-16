#include "chip8.h"

#define FONT_START 0x050
#define FONT_END 0x09F

void setup_chip8(chip8 *c) {
	memset(c->memory, 0, sizeof(c->memory));
	memset(c->display, 0, sizeof(c->display));
	uint16_t fonts[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	for(int i = FONT_START; i <= FONT_END; i++) {
		c->memory[i] = fonts[i - FONT_START];
	}
}
void fetch_opcode(chip8 *c) {
	c->opcode = c->memory[c->pc++] << 8 | c->memory[c->pc++];
}
void execute(chip8 *c) {
	switch(c->opcode & 0xF000) {
		case 0x0000: {
			if((c->opcode & 0x00FF) == 0x00E0) {
				memset(c->display, 0, sizeof(c->display));
			}
			else if((c->opcode & 0x00FF) == 0x00EE) {
				c->pc = c->stack[c->sp];
				c->sp--;
			}
			break;
		}
		case 0x1000: {
			c->pc = c->opcode & 0x0FFF;
			break;
		}
		case 0x6000: {
			uint8_t Vx = (c->opcode & 0x0F00) >> 8;
			c->registers[Vx] = c->opcode & 0x00FF;
			break;
		}
		case 0x7000: {
			uint8_t Vx = (c->opcode & 0x0F00) >> 8;
			c->registers[Vx] += c->opcode & 0x00FF;
			break;
		}
		case 0xA000: {
			c->I = c->opcode & 0x0FFF;
			break;
		}

		case 0xD000: {
			uint8_t x = c->registers[(c->opcode & 0x0F00) >> 8] % 64;
			uint8_t y = c->registers[(c->opcode & 0x0F00) >> 4] % 32;
			uint8_t n = c->opcode & 0x000F;
			c->registers[0xF] = 0;
			for(int i = 0; i < n; i++) {
				if(y + i == 32) break; // end of column
				uint8_t sprite_line = c->memory[c->I + i];
				for(int j = 0; j < 8; j++) {
					if(x + j == 64) break; // end of row
					if(c->display[(y + i) * 64 + x + j] == 1 && ((sprite_line >> (7 - j)) & 1) == 1) {
						c->registers[0xF] = 1;
					}
					c->display[(y + i) * 64 + x + j] ^= (sprite_line >> (7 - j)) & 1;
				}
			}

		}
	}
}