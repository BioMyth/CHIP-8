#include "CPU.h"

template<typename pointerType, typename OPCodeType>

void CPU::fetchDecodeCycle(float delta) {
    accumulator += delta;
    timer += delta;
    renderTimer += delta;

    if (update) {
        updateScreen();
        update = false;
    }

    if (timer >= 1000.f / 60) {
        timer -= 1000.f / 60;
        if (this->DELAY)
            this->DELAY--;
        if (this->SOUND)
            this->SOUND--;
    }

    if (accumulator >= 1000.f / clock) {
        accumulator -= 1000.f / clock;
        updateInput();
        step();
    }
}

void CPU::step() {

    if (this->SOUND) /*Play Sound*/;

    current = RAM[PC++] << 8 | RAM[PC++];

    ::std::function<void(opcode_size, Chip8&)> * func = nullptr;
    if (func = codes.keyExists(current & 0xFFFF));
    else if (func = codes.keyExists(current & 0xF0FF));
    else if (func = codes.keyExists(current & 0xF00F));
    else if (func = codes.keyExists(current & 0xF000));
    else
        throw std::runtime_error("Invalid instruction");
    (*func)(current, *this);

#ifdef EMU_DEBUG
    output << "0x" << std::hex << static_cast<unsigned int>(current) << "\t" << std::hex << PC << "\t" << std::hex << STACK[SP] << std::endl;
#endif
}