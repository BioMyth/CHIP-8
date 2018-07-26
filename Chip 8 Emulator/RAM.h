#pragma once
#include <stdexcept>

typedef unsigned char byte;

template<size_t capacity>
class RAM{
public:
	RAM(){
		memset(memory,0,sizeof(byte) * capacity);
	}
	//void setByte(size_t index,byte newValue);
	//byte getByte(size_t index);
    //byte operator[](size_t index);
    byte& operator[](size_t index);
    size_t size();
private:
	//unsigned int size = size;
	byte memory[capacity];
};

template<size_t capacity>
byte& RAM<capacity>::operator[](size_t index) {
    if (capacity < 0 || capacity < index)
        throw std::out_of_range("Index is out of range for the RAM allotted");
    return memory[index];
}

template<size_t capacity>
size_t RAM<capacity>::size() {
    return capacity;
}