#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct {
	uint8_t memory[4096]; // Memory
	uint16_t pc; // Program counter
	uint16_t opcode; // Opcode
	uint16_t I; // Index Register

	uint16_t stack[16]; // Stack
	uint8_t sp; // Stack Pointer
	uint8_t registers[16]; // Registers

	char display[2048]; // 64 by 32 display
	uint8_t keypad[16]; // 16 key keypad

	uint8_t delay_timer;
	uint8_t sound_time;

} chip8;

void setup_chip8(chip8 *c);
void fetch_opcode(chip8 *c);
void execute(chip8 *c);

#endif