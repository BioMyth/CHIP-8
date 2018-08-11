#include <Windows.h>
#include "Chip8.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Chip8 emu;
    emu.loadProgram("Games/invaders.c8");
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    if (SUCCEEDED(CoInitialize(NULL))) {
        {
            emu.start();
        }
        CoUninitialize();
    }
    return 0;
}