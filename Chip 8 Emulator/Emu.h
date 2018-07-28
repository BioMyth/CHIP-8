#pragma once
#include <Windows.h>
#include <chrono>

#include "GPU.h"
#include "Input.h"
#include "RAM.h"

typedef std::chrono::duration<float, std::milli> TimeStamp;

class Emu {
public:
    Emu(size_t RAMSize, float resX, float resY);
    ~Emu();

    virtual void loadProgram(char* name) = 0;
    virtual void cycle(float delta) = 0;

    void start();

    //virtual void display() = 0;
    void updateScreen();
    //void saveState();
    //void loadState();
    void emuControls();

    bool isKeyPressed(char key);


    HRESULT createWindow();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:

    bool finished;
    bool paused;

    // RAM
    RAM RAM;
    // Display system
    GPU gpu;
    // Input system
    Input input;

    // Window
    HWND hwnd;
};