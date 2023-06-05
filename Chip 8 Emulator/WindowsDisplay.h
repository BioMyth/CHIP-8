#pragma once
#ifdef _WIN32
#ifndef WINDISPLAY
#define WINDISPLAY
#include "Display.h"
#include "WindowHandler.h"
#include "COORD.h"

typedef unsigned short opcode_size;
typedef unsigned char byte;
typedef unsigned short mem_pointer;

class WindowsDisplay : Display {
public:
    void displayCurrentBuffer() {
        byte lineBuffer[xResolution + 1];
        lineBuffer[xResolution]='\0';
        COORD pos={0,0};
		SetConsoleCursorPosition(hOut,pos);
        for (int y = 0; y < yResolution; y++)
            pos.Y=y;
            for(int x=0; x < xResolution; x++){
                Pixel* currentPixel = getPixel(x, y);
                lineBuffer[x] = (isPixelBlack(*currentPixel) ? (219) : ' ');
            }
            SetConsoleCursorPosition(hOut,pos);
            WriteFile(hOut, lineBuffer, xResolution, NULL, NULL);
			pos.Y = 33;
			SetConsoleCursorPosition(hOut, pos);
		}
    };
    bool isPixelBlack(const Pixel pixel){
      return (pixel.r == 0 && pixel.g == 0 && pixel.b == 0);
    };
private:
	HWND console;
	RECT r;
	HANDLE hOut;
    CONSOLE_SCREEN_BUFFER_INFO SBInfo;
    COORD NewSBSize;
	CONSOLE_CURSOR_INFO CursorInfo;
};

#endif
#endif
