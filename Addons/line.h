#pragma once
#include <d2d1.h>
#include <DirectXMath.h>

namespace line {
    // Single declaration for line drawing
    inline void put(ID2D1RenderTarget* renderTarget, ID2D1SolidColorBrush* brush,
    float x1, float y1, float x2, float y2, float thickness = 1.0f)
    {
        if (!renderTarget || !brush) return;
        renderTarget->DrawLine(
            D2D1::Point2F(x1, y1),
            D2D1::Point2F(x2, y2),
            brush,
            thickness
        );
    }
}

/*
Usage examples:
// Draw a white line at (100,100) with default thickness (1.0)
line::put(d2dRenderTarget, whiteBrush, 100.0f, 100.0f);

// Draw a red line at (200,150) with thickness 3.0
line::put(d2dRenderTarget, whiteBrush, 200.0f, 150.0f, 3.0f, D2D1::ColorF::Red);

// Draw a blue line at (300,200) with thickness 5.0
line::put(d2dRenderTarget, whiteBrush, 300.0f, 200.0f, 5.0f, D2D1::ColorF::Blue);
*/