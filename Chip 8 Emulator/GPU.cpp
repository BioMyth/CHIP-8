#include "GPU.h"



GPU::GPU() : factory(nullptr), renderTarget(nullptr), mainBrush(nullptr), buffer(nullptr) {};

GPU::~GPU() {
    SafeRelease(&factory);
    SafeRelease(&renderTarget);
    SafeRelease(&mainBrush);
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
#ifdef EMU_DEBUG
            hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Magenta), &mainBrush);
#else
            hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &mainBrush);
#endif // EMU_DEBUG

            
        }
    }
    return hr;
}

void GPU::DiscardDeviceResources() {
    SafeRelease(&renderTarget);
    SafeRelease(&mainBrush);
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
        float width = static_cast<float>(rtSize.width);
        float height = static_cast<float>(rtSize.height);

        float scaleX = width / static_cast<float>(resX);
        float scaleY = height / static_cast<float>(resY);

        for (size_t y = 0; y < resY; y++) {
            for (size_t x = 0; x < resX; x++) {
                D2D1_RECT_F rect = D2D1::RectF(scaleX * x - 1, scaleY * y - 1, scaleX * (x + 1) + 1, scaleY * (y + 1) + 1);
                mainBrush->SetColor(D2D1::ColorF(buffer[y][x]));
                renderTarget->FillRectangle(&rect, mainBrush);
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
    if (CreateDeviceIndependantResources() != S_OK)
        throw std::runtime_error("GPU could not create Device independant resources");
}

void GPU::setBuffer(uint32_t **&newBuffer) {
    if (buffer != nullptr)
        ClearBuffer();
    buffer = newBuffer;
}

void GPU::InitializeBuffer() {
    if (buffer != nullptr) ClearBuffer();
    
    buffer = new uint32_t *[resY];
    
    for (size_t y = 0; y < resY; y++) {
    
        buffer[y] = new uint32_t [resX];
        for (size_t x = 0; x < resX; x++)
#ifdef EMU_DEBUG
            buffer[y][x] = D2D1::ColorF::Magenta;
#else
            buffer[y][x] = D2D1::ColorF::Black;
#endif // EMU_DEBUG


    }
}

void GPU::ClearBuffer() {
    if (buffer == nullptr) return;
    for (size_t y = 0; y < resY; y++)
        delete[] buffer[y];
    delete[] buffer;
    buffer = nullptr;
}

void GPU::setResolution(size_t x, size_t y) {
    resX = x;
    resY = y;
    InitializeBuffer();
}

size_t GPU::getResolutionX() {
    return resX;
}

size_t GPU::getResolutionY() {
    return resY;
}