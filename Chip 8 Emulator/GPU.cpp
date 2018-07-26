#include "GPU.h"



GPU::GPU() : m_hwnd(nullptr), factory(nullptr), renderTarget(nullptr), whiteBrush(nullptr), redBrush(nullptr) {};

GPU::~GPU() {
    SafeRelease(&factory);
    SafeRelease(&renderTarget);
    SafeRelease(&whiteBrush);
    SafeRelease(&redBrush);
}

void GPU::RunMessageLoop() {
    MSG msg;
    while (GetMessage(&amp;msg, NULL, 0, 0)) {
        TranslateMessage(&amp;msg);
        DispatchMessage(&amp;msg);
    }
}

HRESULT GPU::Initialize() {
    HRESULT hr = CreateDeviceIndependantResources();
    if (SUCCEEDED(hr)) {
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = GPU::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = CLASSNAME;
        RegisterClass(&amp;wcex);

        // Because the CreateWindow function takes its size in pixels,
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;

        // The factory returns the current system DPI. This is also the value it will use
        // to create its own windows.
        factory->GetDesktopDpi(&amp;dpiX, &amp;dpiY);

        // Create the window
        m_hwnd = CreateWindow(
            CLASSNAME,
            L"CHIP8 Direct2D!!!!",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX * 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY * 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this);
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr)) {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }
    return hr;
}

HRESULT GPU::CreateDeviceIndependantResources() {
    HRESULT hr = S_OK;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &amp;factory);
    return hr;
}

HRESULT GPU::CreateDeviceResources() {
    HRESULT hr = S_OK;
    if (!renderTarget) {
        RECT rc;
        GetClientRect(m_hwnd, &amp;rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &amp; renderTarget);
        if (SUCCEEDED(hr)) {
            hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &amp; whiteBrush);
        }
        if (SUCCEEDED(hr)) {
            hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &amp; redBrush);
        }
    }
    return hr;
}

void GPU::DiscardDeviceResources() {
    SafeRelease(&amp;renderTarget);
    SafeRelease(&amp;whiteBrush);
    SafeRelease(&amp;redBrush);
}

LRESULT CALLBACK GPU::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        GPU *gpu = (GPU *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(gpu)
        );

        result = 1;
    }
    else
    {
        GPU *gpu = reinterpret_cast<GPU *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (gpu)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                gpu->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                gpu->OnRender();
                ValidateRect(hwnd, NULL);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

HRESULT GPU::OnRender() {
    HRESULT hr = S_OK;
    hr = CreateDeviceResources();
    if (SUCCEEDED(hr)) {
        renderTarget->BeginDraw();
        renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        D2D1_SIZE_F rtSize = renderTarget->GetSize();

        // Draw a grid background.
        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);

        for (int x = 0; x < width; x += 10)
        {
            renderTarget->DrawLine(
                D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                whiteBrush,
                0.5f
            );
        }

        for (int y = 0; y < height; y += 10)
        {
            renderTarget->DrawLine(
                D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                whiteBrush,
                0.5f
            );
        }

        // Draw two rectangles.
        D2D1_RECT_F rectangle1 = D2D1::RectF(
            rtSize.width / 2 - 50.0f,
            rtSize.height / 2 - 50.0f,
            rtSize.width / 2 + 50.0f,
            rtSize.height / 2 + 50.0f
        );

        D2D1_RECT_F rectangle2 = D2D1::RectF(
            rtSize.width / 2 - 100.0f,
            rtSize.height / 2 - 100.0f,
            rtSize.width / 2 + 100.0f,
            rtSize.height / 2 + 100.0f
        );
        renderTarget->FillRectangle(&amp;rectangle1, whiteBrush);
        renderTarget->FillRectangle(&amp;rectangle2, redBrush);
        hr = renderTarget->EndDraw();
    }
    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        DiscardDeviceResources();
    }
    return hr;
}

void GPU::OnResize(UINT width, UINT height) {
    if (renderTarget) {
        renderTarget->Resize(D2D1::SizeU(width, height));
    }
}