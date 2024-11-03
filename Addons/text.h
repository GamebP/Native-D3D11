#pragma once
#include <iostream>
#include <string>
#include <cstdarg>
#include <d2d1.h>
#include <dwrite.h>

namespace text {
    // The buffer size for the formatted string
    constexpr size_t bufferSize = 512;

    // Console output function
    inline void put(const wchar_t* format, ...) {
        wchar_t buffer[bufferSize];
        va_list args;

        va_start(args, format);
        vswprintf(buffer, bufferSize, format, args);
        va_end(args);

        std::wcout << buffer << std::endl;
    }

    // Drawing function for D2D
    inline void draw(ID2D1RenderTarget* renderTarget, ID2D1SolidColorBrush* brush, 
        IDWriteTextFormat* textFormat, const wchar_t* format, float x, float y, ...) 
    {
        if (!renderTarget || !brush || !textFormat) return;

        // Format the string
        wchar_t buffer[bufferSize];
        va_list args;
        va_start(args, y);
        vswprintf(buffer, bufferSize, format, args);
        va_end(args);

        // Get render target size
        D2D1_SIZE_F size = renderTarget->GetSize();
        float textWidth = size.width * 0.5f;   // Use 50% of window width
        float textHeight = size.height * 0.1f;  // Use 10% of window height

        // Draw the text
        renderTarget->DrawTextW(
            buffer,
            wcslen(buffer),
            textFormat,
            D2D1::RectF(x, y, x + textWidth, y + textHeight),
            brush
        );
    }
}

/*
Usage examples:
// Console output
text::put(L"Hello World");
text::put(L"Value: %d", 42);

// Drawing text on screen
text::draw(d2dRenderTarget, whiteBrush, textFormat, L"Hello World", 5.0f, 5.0f);
text::draw(d2dRenderTarget, whiteBrush, textFormat, L"Score: %d", 5.0f, 5.0f, 100);
*/