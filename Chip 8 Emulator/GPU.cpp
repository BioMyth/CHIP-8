#include "GPU.h"



GPU::GPU() : factory(nullptr), renderTarget(nullptr), whiteBrush(nullptr), redBrush(nullptr) {};

GPU::~GPU() {
    SafeRelease(&factory);
    SafeRelease(&renderTarget);
    SafeRelease(&whiteBrush);
    SafeRelease(&redBrush);
}

HRESULT GPU::Initialize() {
    HRESULT hr = CreateDeviceIndependantResources();
    return hr;
}

HRESULT GPU::CreateDeviceIndependantResources() {
    HRESULT hr = S_OK;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
    return hr;
}

HRESULT GPU::CreateDeviceResources(HWND hwnd) {
    HRESULT hr = S_OK;
    if (!renderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, size),
            & renderTarget);
        if (SUCCEEDED(hr)) {
            hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush);
        }
        if (SUCCEEDED(hr)) {
            hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &redBrush);
        }
    }
    return hr;
}

void GPU::DiscardDeviceResources() {
    SafeRelease(&renderTarget);
    SafeRelease(&whiteBrush);
    SafeRelease(&redBrush);
}

HRESULT GPU::OnRender(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    HRESULT hr = S_OK;
    hr = CreateDeviceResources(hwnd);
    if (SUCCEEDED(hr)) {
        renderTarget->BeginDraw();
        renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        D2D1_SIZE_F rtSize = renderTarget->GetSize();

        // Draw a grid background.
        int width = rtSize.width;
        int height = rtSize.height;

        float scaleX = width / resX;
        float scaleY = height / resY;

        /*
        for (int x = 0; x < resX; x ++)
        {
            renderTarget->DrawLine(
                D2D1::Point2F(static_cast<FLOAT>(x) * scaleX, 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x) * scaleX, resY * scaleY),
                whiteBrush,
                0.5f
            );
        }*/
        /*
        for (int y = 0; y < resY; y ++)
        {
            renderTarget->DrawLine(
                D2D1::Point2F(0.0f, static_cast<FLOAT>(y) * scaleY),
                D2D1::Point2F(resX * scaleX, static_cast<FLOAT>(y) * scaleY),
                whiteBrush,
                0.5f
            );
        }
        */
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                if (buffer[y][x]) {
                    D2D1_RECT_F rect = D2D1::RectF(scaleX * x - 1, scaleY * y - 1, scaleX * (x + 1) + 1, scaleY * (y + 1) + 1);
                    renderTarget->FillRectangle(&rect, whiteBrush);
                }
            }
        }
        hr = renderTarget->EndDraw();
    }
    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        DiscardDeviceResources();
    }
    return hr;
}

void GPU::OnResize(HWND hwnd, WPARAM wParam, LPARAM lParam) {

    if (renderTarget) {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        renderTarget->Resize(D2D1::SizeU(width, height));
    }
}

void GPU::OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    //HRESULT hr = CreateDeviceIndependantResources();
    //return hr;
    if (CreateDeviceIndependantResources() != S_OK)
        throw std::runtime_error("GPU could not create Device independant resources");
    /*LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    GPU *gpu = (GPU *)pcs->lpCreateParams;

    ::SetWindowLongPtrW(
        hwnd,
        GWLP_USERDATA,
        PtrToUlong(gpu)
    );*/
}

void GPU::setBuffer(bool newBuffer[32][64]) {
    memcpy(buffer, newBuffer, 32 * 64);
}

void GPU::setResolution(float x, float y) {
    resX = x;
    resY = y;
}

float GPU::getResolutionX() {
    return resX;
}

float GPU::getResolutionY() {
    return resY;
}