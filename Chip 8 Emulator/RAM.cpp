#include "RAM.h"

RAM::RAM(size_t capacity):capacity(capacity) {
    memory = new byte[capacity];
    memset(memory, 0, sizeof(byte) * capacity);
}

RAM::~RAM() {
    delete[] memory;
}

byte& RAM::operator[](size_t index) {
    if (capacity < 0 || capacity < index)
        throw std::out_of_range("Index is out of range for the RAM allotted");
    return memory[index];
}

size_t RAM::size() {
    return capacity;
}