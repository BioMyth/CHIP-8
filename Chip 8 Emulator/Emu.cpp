#include "Emu.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define CLASSNAME L"D2DGPUDemo"

Emu::Emu(size_t RAMSize, float resX, float resY): finished(true), paused(true), RAM(RAMSize) {
    gpu.setResolution(resX, resY);
};

Emu::~Emu() {}

bool Emu::isKeyPressed(char key) {
    return input[key];
}

void Emu::emuControls() {
    if (isKeyPressed('0')) finished = true;
    if (isKeyPressed('9')) paused = true;
    if (paused) {
        if (isKeyPressed('8'))
            paused = false;
    }
    //save state
    //load state
}

void Emu::start() {
    this->finished = false;
    this->paused = false;
    createWindow();
    //gpu.Initialize();
    //input.Initialize(hwnd);
    //gpu.redrawScreen();

    MSG msg = MSG();
    TimeStamp lastTime = std::chrono::steady_clock::now().time_since_epoch();
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        emuControls();
        TimeStamp now = std::chrono::steady_clock::now().time_since_epoch();
        if (!paused)
            cycle((now - lastTime).count());

        lastTime = now;
        if (finished)
            PostQuitMessage(0);
    }
}

void Emu::updateScreen() {
    //if (UpdateWindow(hwnd));
        //throw std::runtime_error("Could not send update window message");
    if (!InvalidateRect(hwnd, NULL, true));
}

HRESULT Emu::createWindow() {
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Emu::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = HINST_THISCOMPONENT;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = CLASSNAME;

    RegisterClassEx(&wcex);

    // Create the window
    hwnd = CreateWindow(
        CLASSNAME,
        L"CHIP8 Direct2D!!!!",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<UINT>(ceil(gpu.getResolutionX()) * 15),
        static_cast<UINT>(ceil(gpu.getResolutionY()) * 15),
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this);

    HRESULT hr = hwnd ? S_OK : E_FAIL;
    
    if (SUCCEEDED(hr)) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
        UpdateWindow(hwnd);
    }
    
    return hr;
}

LRESULT CALLBACK Emu::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        Emu *emu = (Emu *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(emu)
        );
        emu->gpu.OnCreate(hwnd, wParam, lParam);
        emu->input.OnCreate(hwnd, wParam, lParam);
        result = 1;
    }
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    Emu *emu = reinterpret_cast<Emu *>(static_cast<LONG_PTR>(
        ::GetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA
        )));

    if (!emu) return DefWindowProc(hwnd, message, wParam, lParam);

    bool wasHandled = false;
    switch (message)
    {
    case WM_SIZE:
        emu->gpu.OnResize(hwnd, wParam, lParam);
        result = 0;
        wasHandled = true;
        break;

    case WM_DISPLAYCHANGE:
        InvalidateRect(hwnd, NULL, FALSE);
        result = 0;
        wasHandled = true;
        break;

    case WM_PAINT:
        emu->gpu.OnRender(hwnd, wParam, lParam);
        ValidateRect(hwnd, NULL);
        result = 0;
        wasHandled = true;
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        wasHandled = true;
        break;
    case WM_INPUT:
        result = emu->input.OnInput(hwnd, wParam, lParam);
        wasHandled = true;
        break;
    }

    if (!wasHandled)
    {
        result = DefWindowProc(hwnd, message, wParam, lParam);
    }

    return result;
}