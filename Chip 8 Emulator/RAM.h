#pragma once
#include <stdexcept>

typedef unsigned char byte;

class RAM{
public:
    RAM(size_t capacity);
    ~RAM();

    byte& operator[](size_t index);
    size_t size();
private:
    size_t capacity;
	byte *memory;
};

