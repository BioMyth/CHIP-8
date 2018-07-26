#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

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
/*
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&amp;__ImageBase)
#endif*/
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&amp;__ImageBase)
#endif

#define CLASSNAME L"D2DGPUDemo"

class GPU {
public:
    GPU();
    ~GPU();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();
private:
    // Initialize device-independent resources
    HRESULT CreateDeviceIndependantResources();
    // Initialize device-dependant resources
    HRESULT CreateDeviceResources();

    //Release device-dependent resources
    void DiscardDeviceResources();

    // Draw content
    HRESULT OnRender();
    // Resize the render target
    void OnResize(UINT width, UINT height);
    
    // The windows procedure
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    HWND m_hwnd;
    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* renderTarget;
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* redBrush;
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    if (SUCCEEDED(CoInitialize(NULL))) {
        {
            GPU gpu;
            if (SUCCEEDED(gpu.Initialize())) {
                gpu.RunMessageLoop();
            }
        }
        CoUninitialize();
    }
    return 0;
}