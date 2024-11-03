#pragma once
#include <d2d1.h>
#include <d2d1helper.h>

namespace Colors {
    namespace Get {
        static D2D1::ColorF White() { return D2D1::ColorF(D2D1::ColorF::White); }
        static D2D1::ColorF Black() { return D2D1::ColorF(D2D1::ColorF::Black); }
        static D2D1::ColorF Red() { return D2D1::ColorF(D2D1::ColorF::Red); }
        static D2D1::ColorF Green() { return D2D1::ColorF(D2D1::ColorF::Green); }
        static D2D1::ColorF Blue() { return D2D1::ColorF(D2D1::ColorF::Blue); }
        static D2D1::ColorF Yellow() { return D2D1::ColorF(D2D1::ColorF::Yellow); }
        
        // Additional Colors
        static D2D1::ColorF Orange() { return D2D1::ColorF(1.0f, 0.647f, 0.0f, 1.0f); }
        static D2D1::ColorF Purple() { return D2D1::ColorF(0.5f, 0.0f, 0.5f, 1.0f); }
        static D2D1::ColorF Pink() { return D2D1::ColorF(1.0f, 0.412f, 0.706f, 1.0f); }
        static D2D1::ColorF Cyan() { return D2D1::ColorF(0.0f, 1.0f, 1.0f, 1.0f); }
    }

    namespace UI {
        static D2D1::ColorF Background() { return D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f); }
        static D2D1::ColorF TextPrimary() { return D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f); }
        static D2D1::ColorF TextSecondary() { return D2D1::ColorF(0.7f, 0.7f, 0.7f, 1.0f); }
        static D2D1::ColorF Highlight() { return D2D1::ColorF(0.0f, 0.5f, 1.0f, 1.0f); }
    }

    namespace Game {
        static D2D1::ColorF Health() { return D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f); }
        static D2D1::ColorF Damage() { return D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f); }
        static D2D1::ColorF Shield() { return D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f); }
        static D2D1::ColorF Warning() { return D2D1::ColorF(1.0f, 0.843f, 0.0f, 1.0f); }
    }

    namespace Trans {
        static D2D1::ColorF White() { return D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f); }
        static D2D1::ColorF Black() { return D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f); }
        static D2D1::ColorF Red() { return D2D1::ColorF(1.0f, 0.0f, 0.0f, 0.5f); }
    }
}

/*
// Basic colors
hr = d2dRenderTarget->CreateSolidColorBrush(Colors::Get::Red(), &redBrush);
line::put(d2dRenderTarget, redBrush, 200.0f, 150.0f, 3.0f, Colors::Get::Blue());

// UI colors
text::put(d2dRenderTarget, whiteBrush, L"Hello", Colors::UI::TextPrimary());

// Game colors
line::put(d2dRenderTarget, healthBrush, 200.0f, 150.0f, 3.0f, Colors::Game::Health());

// Transparent colors
box::put(d2dRenderTarget, overlayBrush, 100.0f, 100.0f, Colors::Trans::Black());
*/