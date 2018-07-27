#pragma once
#include <windows.h>

#include <stdlib.h>
#include <stdexcept>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#pragma comment(lib, "d2d1.lib")
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

template<class Interface>
inline void SafeRelease(Interface ** interfaceToRelease) {
    if ((*interfaceToRelease) != nullptr) {
        (*interfaceToRelease)->Release();
        (*interfaceToRelease) = nullptr;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do { if(!(b)) {OutputDebugStringA("Assert: " #b "\n"); }} while(0)
#else
#define Assert(b)
#endif
#endif



class Emu;

class GPU {
public:
    GPU();
    ~GPU();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    void setBuffer(bool newBuffer[32][64]);

    void setResolution(float x, float y);

    float getResolutionX();
    float getResolutionY();

private:
    // Initialize device-independent resources
    HRESULT CreateDeviceIndependantResources();
    // Initialize device-dependant resources
    HRESULT CreateDeviceResources(HWND hwnd);

    //Release device-dependent resources
    void DiscardDeviceResources();

    // Draw content
    HRESULT OnRender(HWND hwnd, WPARAM wParam, LPARAM lParam);
    // Resize the render target
    void OnResize(HWND hwnd, WPARAM wParam, LPARAM lParam);

    void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);

    friend class Emu;
private:
    bool buffer[32][64];
    float resX, resY;
    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* renderTarget;
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* redBrush;
};
