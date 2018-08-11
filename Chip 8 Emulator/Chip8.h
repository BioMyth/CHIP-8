#pragma once
#ifndef _OP_CODES_H_
#define _OP_CODES_H_
#include <fstream>
#include <functional>
#include <chrono>
#include <Windows.h>
#include <winerror.h>
#include <exception>
#include "Emu.h"
#include "OpCodes.h"
//	memory map
//	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
//	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
//	0x200-0xFFF - Program ROM and work RAM


//#define EMU_DEBUG

#ifdef EMU_DEBUG
    #include <iomanip>
#endif // EMU_DEBUG

typedef uint16_t opcode_size;
typedef uint8_t byte;
typedef uint16_t mem_pointer;

class Chip8: public Emu{
public:
    static const size_t clock = 540;
	Chip8();
	~Chip8();

	void loadProgram(char* name);
    void step();
	void fetchDecodeCycle(float delta);
	void updateInput();

    void updateScreen();

private:

	bool update;

	char KEYMAP[16];

	opcode_size current;

	mem_pointer PC;
	mem_pointer INDEX;
	mem_pointer STACK[16];
	
	byte REG[16];
	bool SCREEN[32][64];
	byte SP;
	bool KEY[16];
	byte DELAY;
	byte SOUND;

    float accumulator;
    float timer;

    float renderTimer = 0;

    OpCodes<opcode_size, Chip8> codes;

    void InitializeChip8OpCodes(OpCodes<opcode_size, Chip8> &codes);

#ifdef EMU_DEBUG
    std::ofstream output;
#endif
};
#endif