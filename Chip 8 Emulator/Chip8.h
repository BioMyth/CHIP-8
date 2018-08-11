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
#include "OPCODES.h"
//#include "Chip8-opcodes.h"

//	memory map
//	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
//	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
//	0x200-0xFFF - Program ROM and work RAM

//in hz
//#define CPU_CLOCK 540//600
//#define DEBUG

#ifdef DEBUG
    #include <iomanip>
#endif // DEBUG


#define OPCODE_MAP

typedef uint16_t opcode_size;
typedef uint8_t byte;
typedef uint16_t mem_pointer;

//void InitializeChip8OpCodes(OpCodes<opcode_size, Chip8> &codes);

class Chip8: public Emu{
public:
    static const size_t clock = 540;
	Chip8();
	~Chip8();

	void loadProgram(char* name);
    void step();
	void cycle(float delta);
	//void display();
	void updateInput();

    void updateScreen();

    //friend void InitializeChip8OpCodes(OpCodes<opcode_size, Chip8> &codes);

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

#ifdef DEBUG
    std::ofstream output;
#endif

    /*::std::function<void(opcode_size)> OPCODES[16] ={
            //0x00--
            [this](opcode_size current) {
            switch (current & 0x00FF) {
            case(0xE0):			//Clears the screen.
                for (int y = 0; y < 32; y++) {
                    for (int x = 0; x < 64; x++)
                        this->SCREEN[y][x] = 0;
                }
                this->update = true;
                break;
            case(0xEE):			//Returns from a subroutine.
                this->PC = this->STACK[this->SP--];
                break;
                // NOOP
            case(0x00):
                break;
            default:
                throw std::runtime_error("Could not find opcode(0x00)");
                break;
            }
            this->PC += 2;
        },
            // 0x1NNN
            // Jumps to address NNN.
            [this](opcode_size current) {
            this->PC = current & 0x0FFF;
        },

            // 0x2NNN
            // Calls subroutine at NNN.
            [this](opcode_size current) {
            this->STACK[++this->SP] = this->PC;
            this->PC = current & 0x0FFF;
        },
            // 0x3XRR
            // Skips the next instruction if REG[X] equals NN.
            [this](opcode_size current) {
            this->PC += (this->REG[current >> 8 & 0x0F] == (0x00FF & current) ? 4 : 2);
        },
            // 0x4XRR
            // Skips the next instruction if REG[X] doesn't equal NN.
            [this](opcode_size current) {
            this->PC += (this->REG[current >> 8 & 0x0F] != (0x00FF & current) ? 4 : 2);
        },
            // 0x5XY0
            // Skips the next instruction if REG[X] equals REG[Y].
            [this](opcode_size current) {
            this->PC += (this->REG[current >> 8 & 0x0F] == this->REG[current >> 4 & 0x0F] ? 4 : 2);
        },
            // 0x6XRR
            // Sets REG[X] to NN.
            [this](opcode_size current) {
            this->REG[current >> 8 & 0x0F] = current & 0x00FF;
            this->PC += 2;
        },
            // 0x7XRR
            // Adds NN to REG[X].
            [this](opcode_size current) {
            this->REG[current >> 8 & 0x0F] += current & 0x00FF;
            this->PC += 2;
        },
            // 0x8---
            [this](opcode_size current) {
            switch (current & 0x000F) {
            // 0x8XY0
            // Sets REG[X] to the value of REG[Y].
            case(0x0):
                this->REG[current >> 8 & 0x0F] = this->REG[current >> 4 & 0x0F];
                break;
            // 0x8XY1
            // Sets VX to REG[X] or REG[Y].
            case(0x1):
                this->REG[current >> 8 & 0x0F] |= this->REG[current >> 4 & 0x0F];
                break;
            // 0x8XY2
            // Sets REG[X] to REG[X] and REG[Y].
            case(0x2):
                this->REG[current >> 8 & 0x0F] &= this->REG[current >> 4 & 0x0F];
                break;
            // 0x8XY3
            // Sets REG[X] to REG[X] xor REG[Y].
            case(0x3):
                this->REG[current >> 8 & 0x0F] ^= this->REG[current >> 4 & 0x0F];
                break;

            case(0x4):				// 	Adds REG[Y] to REG[X]. REG[F] is set to 1 when there's a carry, and to 0 when there isn't.
                this->REG[0xF] = (0xFF - this->REG[current >> 8 & 0x0F] < this->REG[current >> 4 & 0x0F]);
                this->REG[current >> 8 & 0x0F] += this->REG[current >> 4 & 0x0F];
                break;

            case(0x5):				// REG[Y] is subtracted from REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
                this->REG[0xF] = (this->REG[current >> 8 & 0x0F] > this->REG[current >> 4 & 0x0F]);
                this->REG[current >> 8 & 0x0F] = this->REG[current >> 8 & 0x0F] - this->REG[current >> 4 & 0x0F];
                break;

            case(0x6):				// Shifts REG[X] right by one. REG[F] is set to the value of the least significant bit of REG[X] before the shift.
                this->REG[0xF] = this->REG[current >> 8 & 0x0F] & 0x01;
                this->REG[current >> 8 & 0x0F] >>= 1;
                break;

            case(0x7):				// Sets REG[X] to REG[Y] minus REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
                this->REG[0xF] = (this->REG[current >> 8 & 0x0F] < this->REG[current >> 4 & 0x0F]);
                this->REG[current >> 8 & 0x0F] = this->REG[current >> 4 & 0x0F] - this->REG[current >> 8 & 0x0F];
                break;

            case(0xE):				//	Shifts REG[X] left by one. REG[F] is set to the value of the most significant bit of REG[X] before the shift
                this->REG[0xF] = this->REG[current >> 8 & 0x0F] >> 7;
                this->REG[current >> 8 & 0x0F] <<= 1;
                break;
            }
            this->PC += 2;
        },

            // Skips the next instruction if REG[X] doesn't equal REG[Y].
            [this](opcode_size current) {
            this->PC += (this->REG[current >> 8 & 0x0F] != this->REG[current >> 4 & 0x0F] ? 4 : 2);
        },

            // Sets INDEX to the address NNN.
            [this](opcode_size current) {
            this->INDEX = 0x0FFF & current;
            this->PC += 2;
        },

            // Jumps to the address NNN plus REG[0].
            [this](opcode_size current) {
            this->PC = (0x0FFF & current) + this->REG[0];
        },

            // 	Sets REG[X] to the result of an & operation on a random number and NN.
            [this](opcode_size current) {
            this->REG[current >> 8 & 0x0F] = rand() & (current & 0x00FF);
            this->PC += 2;
        },

            // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
            // Each row of 8 pixels is read as bit-coded starting from memory location INDEX; INDEX value doesn’t change after this Instruction. 
            // REG[F] is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
            [this](opcode_size current) {
            byte baseX = this->REG[(current >> 8) & 0x0F];
            byte baseY = this->REG[(current >> 4) & 0x0F];
            this->REG[0xF] = 0;
            for (int y = 0; y < (current & 0xF); y++) {
                for (int x = 0; x <= 8; x++) {
                    if (this->SCREEN[baseY + y][baseX + x] && (this->RAM[this->INDEX + y] >> (7 - x) & 0x1)) {
                        this->REG[0xF] = 1;
                    }
                    this->SCREEN[baseY + y][baseX + x] ^= (this->RAM[this->INDEX + y] >> (7 - x) & 0x1);
                }
            }
            //if (REG[0xf])
                this->update = true;
            this->PC += 2;
        },

            [this](opcode_size current) {
            switch (current & 0xFF) {
            case(0x9E):				// Skips the next instruction if the key stored in REG[X] is pressed.
                if (input[KEYMAP[REG[current >> 8 & 0xf]]])//this->KEY[this->REG[current >> 8 & 0xF]])
                    this->PC += 2;
                break;
            case(0xA1):				//	Skips the next instruction if the key stored in REG[X] isn't pressed.
            {
                int tmp = REG[current >> 8 & 0xf];
                tmp;
                if (!input[KEYMAP[tmp]])//this->KEY[this->REG[current >> 8 & 0xF]])
                    this->PC += 2;
            }

                break;
            }
            this->PC += 2;
        },

            // 
            [this](opcode_size current) {
            switch (current & 0xFF) {
            case(0x07):
                this->REG[current >> 8 & 0xF] = this->DELAY;
                break;

            case(0x0A):
            {
                paused = true;
                input.blockForInput([=](char param)->void {
                    for (int i = 0; i < 0xF; i++) {
                        if (param == KEYMAP[i]) {
                            this->REG[current >> 8 & 0xf] = i;
                            paused = false;
                            break;
                        }
                    }
                });
                break;
            }
            case(0x15):
                this->DELAY = this->REG[current >> 8 & 0xF];
                break;

            case(0x18):
                this->SOUND = this->REG[current >> 8 & 0xF];
                break;

            case(0x1E):
                this->INDEX += this->REG[current >> 8 & 0xF];
                break;

            case(0x29):
                this->INDEX = this->REG[(current & 0x0F00) >> 8] * 0x5;
                break;

            case(0x33):
                this->RAM[this->INDEX] = this->REG[current >> 8 & 0xF] / 100;
                this->RAM[this->INDEX + 1] = (this->REG[current >> 8 & 0xF] % 100) / 10;
                this->RAM[this->INDEX + 2] = this->REG[current >> 8 & 0xF] % 10;
                break;

            case(0x55):
                for (int i = 0x0; i <= (current >> 8 & 0xF); i++)
                    this->RAM[this->INDEX + i] = this->REG[i];
                break;

            case(0x65):
                for (int i = 0x0; i <= (current >> 8 & 0xF); i++)
                    this->REG[i] = this->RAM[this->INDEX + i];
                break;
            }
            this->PC += 2;
        }
    };*/
};
#endif