#pragma once
#include <functional>
#include <map>

template<typename opcode_size, typename EType>
class OpCodes {
public:
    ::std::function<void(opcode_size, EType&)>& operator[](opcode_size index) {
        return codes[index];
    }
    ::std::function<void(opcode_size, EType&)>& operator[](int index) {
        return codes[static_cast<opcode_size>(index)];
    }
    ::std::function<void(opcode_size, EType&)>* keyExists(opcode_size key) {
        auto iter = codes.find(key);
        return (iter != codes.end() ? &(iter->second): nullptr);
    }
protected:
    std::map< opcode_size, ::std::function<void(opcode_size, EType&)>> codes;
};