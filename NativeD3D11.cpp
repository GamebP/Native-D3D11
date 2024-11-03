#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <windows.h>
#include <DirectXMath.h>
#include <dwrite.h>
#include <chrono>
#include <string>

#include "./Addons/colors.h"
#include "./Addons/text.h"
#include "./Addons/line.h"
#include "./Addons/box.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

using namespace DirectX;
using namespace D2D1;

// Function declarations
void ResizeTextFormat();
void ReleaseResources();
void RenderFrame();
void RenderFPSOverlay(int fps);
void ResizeWindow(UINT width, UINT height);
HRESULT CreateD2DRenderTarget();  // Add this line
bool isRunning = true;

// Global declarations
HWND HWNDmainWindow = nullptr;
HWND overlayWindow = nullptr;
IDXGISwapChain* swapChain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;
IDWriteFactory* writeFactory = nullptr;
IDWriteTextFormat* textFormat = nullptr;
ID2D1Factory* d2dFactory = nullptr;
ID2D1RenderTarget* d2dRenderTarget = nullptr;
ID2D1HwndRenderTarget* d2dOverlayRenderTarget = nullptr;

ID2D1SolidColorBrush* overlayBrush = nullptr;
ID2D1SolidColorBrush* whiteBrush = nullptr;
ID2D1SolidColorBrush* redBrush = nullptr;

// Global state for resizing and interaction
bool isMouseHeld = false;
bool isFullscreen = false;
float baseFontSize = 24.0f;
float fontSize = baseFontSize;
int windowWidth = 800;
int windowHeight = 600;

inline ID2D1SolidColorBrush* CreateBrush(ID2D1RenderTarget* renderTarget, D2D1::ColorF color) {
    ID2D1SolidColorBrush* brush = nullptr;
    renderTarget->CreateSolidColorBrush(color, &brush);
    return brush;
}

// Function to initialize overlay window
HWND CreateOverlayWindow(HINSTANCE hInstance) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"OverlayWindowClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        L"OverlayWindowClass",
        L"FPS Overlay",
        WS_POPUP,
        10, 10, 300, 50,
        NULL, NULL, hInstance, NULL
    );

    // Set layered window transparency
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    return hwnd;
}

// Initialize Direct2D for the overlay
void InitializeOverlayD2D(HWND hwnd) {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

    RECT rc;
    GetClientRect(hwnd, &rc);
    d2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right, rc.bottom)),
        &d2dOverlayRenderTarget
    );

    d2dOverlayRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &overlayBrush);

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
    writeFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-us", &textFormat
    );
}

// Function to initialize D3D11 and DirectWrite
HRESULT InitializeD3D(HWND hWnd) {
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.Width = 800;
    swapChainDesc.BufferDesc.Height = 600;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0,
        D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &deviceContext
    );

    if (FAILED(hr)) {
        MessageBox(nullptr, L"Failed to create Direct3D device and swap chain", L"Error", MB_OK);
        return hr;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) return hr;

    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) return hr;

    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(windowWidth);
    viewport.Height = static_cast<float>(windowHeight);
    deviceContext->RSSetViewports(1, &viewport);

    // Create D2D and DirectWrite factories - using the same hr variable
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
    if (FAILED(hr)) return hr;

    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&writeFactory)
    );
    if (FAILED(hr)) return hr;

    // Create text format
    hr = writeFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-us",
        &textFormat
    );
    if (FAILED(hr)) return hr;

    // Create D2D render target and white brush
    return CreateD2DRenderTarget();
}

void ResizeTextFormat() {
    if (textFormat) {
        textFormat->Release();
        textFormat = nullptr;
    }
    writeFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &textFormat);
}

// Helper function to create or recreate Direct2D render target
HRESULT CreateD2DRenderTarget() {
    if (d2dRenderTarget) {
        d2dRenderTarget->Release();
        d2dRenderTarget = nullptr;
    }

    IDXGISurface* dxgiBackBuffer;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiBackBuffer);
    if (FAILED(hr)) {
        text::put(L"Failed to get DXGI back buffer!");
        return hr;
    }

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    hr = d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiBackBuffer, &props, &d2dRenderTarget);
    dxgiBackBuffer->Release();

    if (SUCCEEDED(hr)) {
        // Recreate the brushes
        if (whiteBrush) {
            whiteBrush->Release();
            whiteBrush = nullptr;
        }
        if (redBrush) {
            redBrush->Release();
            redBrush = nullptr;
        }
        
        hr = d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush);
        if (FAILED(hr)) {
            text::put(L"Failed to create white brush!");
        }
        
        hr = d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &redBrush);
        if (FAILED(hr)) {
            text::put(L"Failed to create red brush!");
        }
    }
    else {
        text::put(L"Failed to create D2D render target!");
    }

    return hr;
}

void DrawMainWindowText(const wchar_t* text, float x, float y) {
    if (!d2dRenderTarget || !whiteBrush || !textFormat) return;

    d2dRenderTarget->BeginDraw();
    d2dRenderTarget->DrawTextW(
        text,
        wcslen(text),
        textFormat,
        D2D1::RectF(x, y, x + 400.0f, y + 50.0f), // Adjust size for longer text
        whiteBrush
    );
    d2dRenderTarget->EndDraw();
}

// Draw FPS on overlay
void RenderFPSOverlay(int fps) {
    d2dOverlayRenderTarget->BeginDraw();

    d2dOverlayRenderTarget->Clear(D2D1::ColorF(0, 0.0f));

    wchar_t fpsText[32];
    swprintf_s(fpsText, L"FPS: %d", fps);

    d2dOverlayRenderTarget->DrawTextW(
        fpsText,
        wcslen(fpsText),
        textFormat,
        D2D1::RectF(10.0f, 10.0f, 300.0f, 50.0f),
        overlayBrush
    );

    d2dOverlayRenderTarget->EndDraw();
}

void RenderFrame() {
    // Clear the D3D11 render target
    const float color[4] = { 0.0f, 0.0f, 0.2f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, color);

    // Check if render targets are valid
    if (!d2dRenderTarget || !whiteBrush) {
        text::put(L"D2D render target or brush is null!");
        return;
    }

    // Begin D2D drawing
    d2dRenderTarget->BeginDraw();
    if (!d2dRenderTarget) {
        text::put(L"BeginDraw failed!");
        return;
    }

    try {
        // Draw text
        text::draw(d2dRenderTarget, 
            CreateBrush(d2dRenderTarget, Colors::UI::TextPrimary()), 
            textFormat, L"Hello world bro!", 
            5.0f, 5.0f);

        line::put(d2dRenderTarget, 
            CreateBrush(d2dRenderTarget, Colors::Get::Red()), 
            700.0f, 150.0f,
            400.0f, 450.0f,
            3.0f);

        box::put(d2dRenderTarget, 
            CreateBrush(d2dRenderTarget, Colors::UI::Highlight()), 
            100, 100, 
            300, 250, 
            2.0f, Colors::UI::Highlight().a);

        // End D2D drawing
        HRESULT hr;
        hr = d2dRenderTarget->EndDraw();
        if (FAILED(hr)) {
            text::put(L"EndDraw failed!");
            CreateD2DRenderTarget();
        }
    }
    catch (...) {
        text::put(L"Exception during drawing!");
    }

    // Present the frame
    swapChain->Present(1, 0);
}

void ResizeWindow(UINT width, UINT height) {
    if (width == 0 || height == 0) return;

    // Release old views but keep the brush
    if (renderTargetView) {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }
    if (d2dRenderTarget) {
        // Don't release the brush here, just the render target
        d2dRenderTarget->Release();
        d2dRenderTarget = nullptr;
    }

    // Resize swap chain
    HRESULT hr = swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) return;

    // Recreate render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (SUCCEEDED(hr)) {
        hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
        backBuffer->Release();
    }

    // Update viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);

    // Recreate D2D render target
    CreateD2DRenderTarget();

    // Set the render target
    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // Immediately render a frame to prevent flickering
    RenderFrame();
}

// Window procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SIZE:
        if (device != nullptr) {
            windowWidth = LOWORD(lParam);
            windowHeight = HIWORD(lParam);
            
            // Check if window is maximized
            isFullscreen = (wParam == SIZE_MAXIMIZED);

            // Update overlay position based on maximized state
            if (overlayWindow) {
                RECT windowRect;
                GetWindowRect(hWnd, &windowRect);
                if (isFullscreen) {
                    SetWindowPos(overlayWindow, HWND_TOPMOST,
                        windowRect.right - 300, 10,  // Position on the right side
                        300, 50, SWP_NOSIZE | SWP_NOZORDER);
                } else {
                    SetWindowPos(overlayWindow, HWND_TOPMOST,
                        10, 10,  // Default position (left side)
                        300, 50, SWP_NOSIZE | SWP_NOZORDER);
                }
            }
            
            ResizeWindow(windowWidth, windowHeight);
        }
        return 0;
    case WM_DESTROY:
        isRunning = false;
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// Resource cleanup
void ReleaseResources() {
    if (whiteBrush) whiteBrush->Release();
    if (d2dRenderTarget) d2dRenderTarget->Release();
    if (d2dFactory) d2dFactory->Release();
    if (textFormat) textFormat->Release();
    if (writeFactory) writeFactory->Release();
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
    if (deviceContext) deviceContext->Release();
    if (device) device->Release();
    if (overlayBrush) overlayBrush->Release();
    if (d2dOverlayRenderTarget) d2dOverlayRenderTarget->Release();
}

// WinMain function (entry point)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, hInstance, nullptr, nullptr, nullptr, nullptr, L"D3DWindowClass", nullptr };
    RegisterClassEx(&wc);

    // Create the main window
    HWNDmainWindow = CreateWindow(L"D3DWindowClass", L"Direct3D 11 Window", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, nullptr, nullptr, hInstance, nullptr);
    if (!HWNDmainWindow) {
        text::put(L"Failed to create main window!"); // Use text::put for error messages
        MessageBox(nullptr, L"Failed to create main window!", L"Error", MB_OK);
        return -1;
    }

    // Initialize Direct3D and Direct2D resources
    if (FAILED(InitializeD3D(HWNDmainWindow))) {
        text::put(L"Failed to initialize Direct3D!"); // Use text::put for error messages
        MessageBox(nullptr, L"Failed to initialize Direct3D!", L"Error", MB_OK);
        ReleaseResources();
        return -1;
    }

    // Create overlay window for FPS counter
    overlayWindow = CreateOverlayWindow(hInstance);
    if (!overlayWindow) {
        text::put(L"Failed to create FPS overlay window!"); // Use text::put for error messages
        MessageBox(nullptr, L"Failed to create FPS overlay window!", L"Error", MB_OK);
        ReleaseResources();
        return -1;
    }

    ShowWindow(HWNDmainWindow, SW_SHOW);
    ShowWindow(overlayWindow, SW_SHOWNOACTIVATE);
    InitializeOverlayD2D(overlayWindow);

    // Main message loop
    MSG msg = {};
    while (isRunning) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) isRunning = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!isRunning) break;

        // Calculate FPS
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - startTime).count();
        int fps = static_cast<int>(1.0f / deltaTime);
        startTime = currentTime;

        // Render the main window and FPS overlay
        RenderFrame();
        RenderFPSOverlay(fps);

        // Use text::put to output FPS to the console
        text::put(L"Current FPS: %d", fps);
    }

    // Clean up resources and exit
    ReleaseResources();
    UnregisterClass(L"D3DWindowClass", wc.hInstance);
    return 0;
}