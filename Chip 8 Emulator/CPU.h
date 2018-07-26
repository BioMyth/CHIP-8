#pragma once

#include "RAM.h"
template<typename pointerType,typename OPCodeType>
class CPU{
public:
    CPU(float frequency, std::function<void()> *opcodes) :speed(speed), opcodes(opcodes) {};
	void cycle();

private:
	void std::function<void()> *opcodes;
	float speed;
	sizeOfPointers stack[16], PC, INDEX;
	sizeOfOPCodes current;
};

/*void cycle(, ) {

}*/
