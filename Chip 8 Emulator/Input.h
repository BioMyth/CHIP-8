#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <exception>
#include <stdexcept>
#include <functional>
#include <cstdint>

class Input {
public:
    //Input();
    //~Input();

    int OnInput(HWND hwnd, WPARAM wParam, LPARAM lParam);

    void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);

    void blockForInput(const std::function<void(char)> &f);

    bool operator[](char index);
    bool operator[](int index);

private:
    RAWINPUTDEVICE rid;
    HKL locale;
    bool keyState[0xff];
    bool blocked;
    std::function<void(char)> func;
};
