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

inline uint32_t RGBTOCOLOR(uint8_t r,uint8_t g,uint8_t b) {
    uint32_t ret = r & 0xff;
    ret <<= 16;
    ret |= g & 0xff;
    ret <<= 16;
    ret |= b & 0xff;
    return ret;
}


class Emu;

class GPU {
public:
    GPU();
    ~GPU();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    void setBuffer(uint32_t **&newBuffer);

    void setResolution(size_t x, size_t y);

    size_t getResolutionX();
    size_t getResolutionY();

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

    void InitializeBuffer();
    void ClearBuffer();

    friend class Emu;
private:
    uint32_t **buffer;
    size_t resX, resY;
    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* renderTarget;
    ID2D1SolidColorBrush* mainBrush;
};
