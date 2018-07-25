#pragma once

#include "RAM.h"
template<typename sizeOfPointers,typename sizeOfOPCodes>
class CPU{
public:
	CPU(float speed, void *(*opcodes)()):speed(speed),opcodes(opcodes){};
	void cycle();

private:
	void *(*opcodes)();
	float speed;
	sizeOfPointers stack[16], PC, INDEX;
	sizeOfOPCodes current;
};
