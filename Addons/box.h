#pragma once
#include <d2d1.h>
#include <DirectXMath.h>

namespace box {
    // Draw a box with transparent fill and colored outline
    inline void put(ID2D1RenderTarget* renderTarget, ID2D1SolidColorBrush* brush,
        float x1, float y1, float x2, float y2, float thickness = 1.0f, float opacity = 1.0f)
    {
        if (!renderTarget || !brush) return;
        // Draw transparent fill
        brush->SetOpacity(0.1f);
        renderTarget->FillRectangle(
            D2D1::RectF(x1, y1, x2, y2),
            brush
        );
        // Draw colored outline
        brush->SetOpacity(opacity);
        renderTarget->DrawRectangle(
            D2D1::RectF(x1, y1, x2, y2),
            brush,
            thickness
        );
        brush->SetOpacity(1.0f); // Reset opacity
    }
}

/*
Usage:
box::put(d2dRenderTarget, whiteBrush, 100, 100, 300, 250);                // Default transparent box with outline
box::put(d2dRenderTarget, whiteBrush, 100, 100, 300, 250, 2.0f);         // Thick outline
box::put(d2dRenderTarget, whiteBrush, 100, 100, 300, 250, 2.0f, 0.5f);   // Thick semi-transparent outline
*/