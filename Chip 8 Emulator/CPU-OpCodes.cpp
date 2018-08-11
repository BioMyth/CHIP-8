#include "OpCodes.h"
#include "Chip8.h"

void Chip8::InitializeChip8OpCodes(OpCodes<opcode_size, Chip8> &codes) {
    // 0x0000
    // NoOp
    codes[0x0000] = [=](opcode_size current, Chip8 &c8) {};

    // 0x00E0
    // Clears the screen
    codes[0x00E0] = [=](opcode_size current, Chip8 &c8) -> void {
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++)
                c8.SCREEN[y][x] = 0;
        }
        c8.update = true; };

    // 0x00EE
    // Moves up one level in the stack
    codes[0x00EE] = [=](opcode_size current, Chip8 &c8) {
        c8.PC = c8.STACK[c8.SP--]; };

    // 0x1NNN
    // Jumps to address NNN.
    codes[0x1000] = [=](opcode_size current, Chip8 &c8) {
        c8.PC = current & 0x0FFF; };

    // 0x2NNN
    // Calls subroutine at NNN.
    codes[0x2000] = [](opcode_size current, Chip8 &c8) {
        c8.STACK[++c8.SP] = c8.PC;
        c8.PC = current & 0x0FFF; };

    // 0x3XRR
    // Skips the next instruction if REG[X] equals NN.
    codes[0x3000] = [](opcode_size current, Chip8 &c8) {
        c8.PC += (c8.REG[current >> 8 & 0x0F] == (0x00FF & current)) * 2; };

    // 0x4XRR
    // Skips the next instruction if REG[X] doesn't equal NN.
    codes[0x4000] = [](opcode_size current, Chip8 &c8) {
        c8.PC += (c8.REG[current >> 8 & 0x0F] != (0x00FF & current)) * 2; };

    // 0x5XY0
    // Skips the next instruction if REG[X] equals REG[Y].
    codes[0x5000] = [](opcode_size current, Chip8 &c8) {
        c8.PC += (c8.REG[current >> 8 & 0x0F] == c8.REG[current >> 4 & 0x0F]) * 2; };

    // 0x6XRR
    // Sets REG[X] to NN.
    codes[0x6000] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] = current & 0x00FF; };

    // 0x7XRR
    // Adds NN to REG[X].
    codes[0x7000] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] += current & 0x00FF; };

    // 0x8XY0
    // Sets REG[X] to the value of REG[Y].
    codes[0x8000] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] = c8.REG[current >> 4 & 0x0F]; };

    // 0x8XY1
    // Sets VX to REG[X] or REG[Y].
    codes[0x8001] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] |= c8.REG[current >> 4 & 0x0F]; };

    // 0x8XY2
    // Sets REG[X] to REG[X] and REG[Y].
    codes[0x8002] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] &= c8.REG[current >> 4 & 0x0F]; };

    // 0x8XY3
    // Sets REG[X] to REG[X] xor REG[Y].
    codes[0x8003] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] ^= c8.REG[current >> 4 & 0x0F]; };

    // 0x8XY4
    // Adds REG[Y] to REG[X]. REG[F] is set to 1 when there's a carry, and to 0 when there isn't.
    codes[0x8004] = [](opcode_size current, Chip8 &c8) {
        c8.REG[0xF] = (0xFF - c8.REG[current >> 8 & 0x0F] < c8.REG[current >> 4 & 0x0F]);
        c8.REG[current >> 8 & 0x0F] += c8.REG[current >> 4 & 0x0F]; };

    // 0x8XY5
    // REG[Y] is subtracted from REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
    codes[0x8005] = [](opcode_size current, Chip8 &c8) {
        c8.REG[0xF] = (c8.REG[current >> 8 & 0x0F] > c8.REG[current >> 4 & 0x0F]);
        c8.REG[current >> 8 & 0x0F] = c8.REG[current >> 8 & 0x0F] - c8.REG[current >> 4 & 0x0F]; };

    // 0x8XY6
    // Shifts REG[X] right by one. REG[F] is set to the value of the least significant bit of REG[X] before the shift.
    codes[0x8006] = [](opcode_size current, Chip8 &c8) {
        c8.REG[0xF] = c8.REG[current >> 8 & 0x0F] & 0x01;
        c8.REG[current >> 8 & 0x0F] >>= 1; };

    // 0x8XY7
    // Sets REG[X] to REG[Y] minus REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
    codes[0x8007] = [](opcode_size current, Chip8 &c8) {
        c8.REG[0xF] = (c8.REG[current >> 8 & 0x0F] < c8.REG[current >> 4 & 0x0F]);
        c8.REG[current >> 8 & 0x0F] = c8.REG[current >> 4 & 0x0F] - c8.REG[current >> 8 & 0x0F]; };

    // 0x8XYE
    //	Shifts REG[X] left by one. REG[F] is set to the value of the most significant bit of REG[X] before the shift
    codes[0x800E] = [](opcode_size current, Chip8 &c8) {
        c8.REG[0xF] = c8.REG[current >> 8 & 0x0F] >> 7;
        c8.REG[current >> 8 & 0x0F] <<= 1; };

    // 0x9XY0
    // Skips the next instruction if REG[X] doesn't equal REG[Y].
    codes[0x9000] = [](opcode_size current, Chip8 &c8) {
        c8.PC += (c8.REG[current >> 8 & 0x0F] != c8.REG[current >> 4 & 0x0F]) * 2; };

    // 0xANNN
    // Sets INDEX to the address NNN.
    codes[0xA000] = [](opcode_size current, Chip8 &c8) {
        c8.INDEX = 0x0FFF & current; };

    // 0xBNNN
    // Jumps to the address NNN plus REG[0].
    codes[0xB000] = [](opcode_size current, Chip8 &c8) {
        c8.PC = (0x0FFF & current) + c8.REG[0]; };

    // 0xCXNN
    // 	Sets REG[X] to the result of an & operation on a random number and NN.
    codes[0xC000] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0x0F] = rand() & (current & 0x00FF); };

    // 0xDXYN
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
    // Each row of 8 pixels is read as bit-coded starting from memory location INDEX; INDEX value doesn’t change after  Instruction. 
    // REG[F] is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
    codes[0xD000] = [](opcode_size current, Chip8 &c8) {
        byte baseX = c8.REG[(current >> 8) & 0x0F];
        byte baseY = c8.REG[(current >> 4) & 0x0F];
        c8.REG[0xF] = 0;
        for (int y = 0; y < (current & 0xF); y++) {
            for (int x = 0; x < 8; x++) {
                if (c8.SCREEN[baseY + y][baseX + x] && (c8.RAM[c8.INDEX + y] >> (7 - x) & 0x1)) {
                    c8.REG[0xF] = 1;
                }
                c8.SCREEN[baseY + y][baseX + x] ^= (c8.RAM[c8.INDEX + y] >> (7 - x) & 0x1);
            }
        }
        c8.update = true; };


    // 0xEX9A
    // Skips the next instruction if the key stored in REG[X] is pressed.
    codes[0xE09E] = [](opcode_size current, Chip8 &c8) {
        if (c8.input[c8.KEYMAP[c8.REG[current >> 8 & 0xf]]])//c8.KEY[c8.REG[current >> 8 & 0xF]])
            c8.PC += 2; };

    // 0xEXA1
    // Skips the next instruction if the key stored in REG[X] isn't pressed.
    codes[0xE0A1] = [](opcode_size current, Chip8 &c8) {
        if (!c8.input[c8.KEYMAP[c8.REG[current >> 8 & 0xF]]])
            c8.PC += 2; };


    codes[0xF007] = [](opcode_size current, Chip8 &c8) {
        c8.REG[current >> 8 & 0xF] = c8.DELAY; };

    codes[0xF00A] = [](opcode_size current, Chip8 &c8) {
        c8.paused = true;
        Chip8 *tmp = &c8;
        c8.input.blockForInput([=](char param)->void {
            for (int i = 0; i < 0xF; i++) {
                if (param == tmp->KEYMAP[i]) {
                    tmp->REG[current >> 8 & 0xf] = i;
                    tmp->paused = false;
                    break;
                }
            }
        }); };

    codes[0xF015] = [](opcode_size current, Chip8 &c8) {
        c8.DELAY = c8.REG[current >> 8 & 0xF]; };

    codes[0xF018] = [](opcode_size current, Chip8 &c8) {
        c8.SOUND = c8.REG[current >> 8 & 0xF]; };

    codes[0xF01E] = [](opcode_size current, Chip8 &c8) {
        c8.INDEX += c8.REG[current >> 8 & 0xF]; };

    codes[0xF029] = [](opcode_size current, Chip8 &c8) {
        c8.INDEX = c8.REG[(current & 0x0F00) >> 8] * 0x5; };

    codes[0xF033] = [](opcode_size current, Chip8 &c8) {
        c8.RAM[c8.INDEX] = c8.REG[current >> 8 & 0xF] / 100;
        c8.RAM[c8.INDEX + 1] = (c8.REG[current >> 8 & 0xF] % 100) / 10;
        c8.RAM[c8.INDEX + 2] = c8.REG[current >> 8 & 0xF] % 10; };

    codes[0xF055] = [](opcode_size current, Chip8 &c8) {
        for (int i = 0x0; i <= (current >> 8 & 0xF); i++)
            c8.RAM[c8.INDEX + i] = c8.REG[i]; };

    codes[0xF065] = [](opcode_size current, Chip8 &c8) {
        for (int i = 0x0; i <= (current >> 8 & 0xF); i++)
            c8.REG[i] = c8.RAM[c8.INDEX + i]; };
}