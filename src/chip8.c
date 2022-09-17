#include "chip8.h"
#define FONT_START 0x050
#define FONT_END 0x09F

void setup_chip8(chip8 *c) {
	struct timeval tm;
    gettimeofday(&tm, NULL);
    srandom(tm.tv_sec + tm.tv_usec * 1000000ul);
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
				c->pc = c->stack[c->sp--];
			}
			break;
		}
		case 0x1000: {
			c->pc = c->opcode & 0x0FFF;
			break;
		}
		/*
		case 0x2000: {
			c->stack[c->sp] = c->pc;
			c->sp++;
			c->pc = c->opcode & 0x0FFF;
			break;

		}
		*/
		case 0x3000: {
			if(c->registers[(c->opcode & 0x0F00) >> 8] == (c->opcode & 0x00FF)) c->pc += 2;
			break;
		}
		case 0x4000: {
			if(c->registers[(c->opcode & 0x0F00) >> 8] != (c->opcode & 0x00FF)) c->pc += 2;
			break;
		}
		case 0x5000: {
			if(c->registers[(c->opcode & 0x0F00) >> 8] == c->registers[c->opcode & 0x00F0]) c->pc += 2;
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
		case 0x8000: {
			uint8_t x = (c->opcode & 0x0F00) >> 8;
			uint8_t y = (c->opcode & 0x00F0) >> 4;
			switch(c->opcode & 0x000F) {
				case 0x0000: {
					c->registers[x] = c->registers[y];
					break;
				}
				case 0x0001: {
					c->registers[x] |= c->registers[y];
					break;
				}
				case 0x0002: {
					c->registers[x] &= c->registers[y];
					break;
				}
				case 0x0003: {
					c->registers[x] ^= c->registers[y];
					break;
				}
				case 0x0004: {
					if(c->registers[x] + c->registers[y] > 255) {
						c->registers[0xF] = 1;
						c->registers[x] = (c->registers[x] + c->registers[y]) & 0xFF;
						break;
					};
					c->registers[0xF] = 0;
					c->registers[x] += c->registers[y];
					break;
				}
				case 0x0005: {
					c->registers[0xF] = 0;
					if(c->registers[x] > c->registers[y]) c->registers[0xF] = 1;
					c->registers[x] -= c->registers[y];
					break;
				}
				case 0x0006: {
					if((c->registers[x] & 1) == 1) c->registers[0xF] = 1;
					c->registers[x] >>= 1;
					break;
				}
				case 0x0007: {
					c->registers[0xF] = 0;
					if(c->registers[y] > c->registers[x]) c->registers[0xF] = 1;
					c->registers[x] = c->registers[y] = c->registers[x];
					break;
				}
				case 0x000E: {
					c->registers[0xF] = c->registers[x] >> 7;
					c->registers[x] <<= 1;
					break;
				}
				break;
			}
		}
		case 0x9000: {
			if(c->registers[(c->opcode & 0x0F00) >> 8] != c->registers[(c->opcode & 0x00F0) >> 4]) c->pc += 2;
			break;
		}
		case 0xA000: {
			c->I = c->opcode & 0x0FFF;
			break;
		}
		case 0xB000: {
			c->pc = (c->opcode & 0x0FFF) + c->registers[0];
		}
		case 0xC000: {
			int random_value = arc4random_uniform(256);
			c->registers[c->opcode & 0x0F00 >> 8] = random_value & (c->opcode & 0x00FF);

		}
		case 0xD000: {
			uint8_t x = c->registers[(c->opcode & 0x0F00) >> 8] % 64;
			uint8_t y = c->registers[(c->opcode & 0x00F0) >> 4] % 32;
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
			break;
		}
		case 0xE000: {
			uint8_t key = c->opcode & 0x0F00 >> 8;
			if((c->opcode & 0x00FF) == 0x009E) {
				if(c->keypad[key] == 1) c->pc += 2;
			}
			if((c->opcode & 0x00FF) == 0x00A1) {
				if(c->keypad[key] == 0) c->pc += 2;
			}
			break;
		}
		case 0xF000: {
			uint8_t x = c->registers[(c->opcode & 0x0F00) >> 8];
			switch(c->opcode & 0x00FF) {
				case 0x0007: {
					c->registers[x] = c->delay_timer;
					break;
				}
				case 0x000A: {
					for(int i = 0x0; i < 0xF; i++){
						if(c->keypad[i] == 1) {
							c->registers[x] = i;
							break;
						}
					}
				}
				case 0x0015: {
					c->delay_timer = c->registers[x];
					break;
				}
				case 0x0018: {
					c->sound_timer = c->registers[x];
					break;
				}
				case 0x001E: {
					c->I += c->registers[x];
					break;
				}
				case 0x0029: {
					c->I = FONT_START + (0x5 * c->registers[x]);
					break;
				}
				case 0x0033: {
					uint8_t number = c->registers[x];
					printf("%d\n", number);
					c->memory[c->I] = number / 100;
					c->memory[c->I + 1] = (number / 10) % 10;
					c->memory[c->I + 2] = number % 10;
					printf("(%d, %d, %d)\n", c->memory[c->I], c->memory[c->I + 1], c->memory[c->I + 2]);
					break;
				}
				case 0x0055: {
					for(int i = 0; i <= x; i++) {
						c->memory[c->I + i] = c->registers[i];
					}
					break;
				}
				case 0x0065: {
					for(int i = 0; i <= x; i++) {
						c->registers[i] = c->memory[c->I + i];
					}
					break;
				}
			}
		}
	}
}


