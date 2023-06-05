#pragma once
#include <stdexcept>

typedef unsigned char byte;

template<unsigned int T>
class RAM{
public:
	RAM(){
		memset(memory, 0, sizeof(byte) * size);
	}
	void setByte(unsigned int index,byte newValue);
	byte getByte(unsigned int index);

private:
	unsigned int size = T;
	byte memory[T];
};

template<unsigned int T>
void RAM<T>::setByte(unsigned int index, byte newValue){
	if( index > 0 && index < size)
		throw std::out_of_range("Index for memory write out of range");
	memory[index] = newValue;
}

template<unsigned int T>
byte RAM<T>::getByte(unsigned int index){
	if( index > 0 && index < size)
		throw std::out_of_range("Index for memory read out of range");
	return memory[index];
}
