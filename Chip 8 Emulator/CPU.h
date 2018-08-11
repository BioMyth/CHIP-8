#pragma once

#include "RAM.h"
#include <functional>

template<typename pointerType,typename OPCodeType>
class CPU{
public:
    CPU(int frequency, OpCodes<pointerType> asdf) :speed(1000.f / frequency), opcodes(opcodes) {};
    virtual void cycle(float delta) = 0;
    virtual OPCodeType getOpcode() = 0;
private:
	void std::function<void()> *opcodes;
	float speed;
};


