# Direct3D 11 Overlay Application
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![DirectX](https://img.shields.io/badge/DirectX_11-107C10?style=for-the-badge)

A lightweight Windows application showcasing Direct3D 11 and Direct2D integration with real-time overlay capabilities.

## 🚀 Features

- 🎮 Direct3D 11 and Direct2D rendering
- 📊 Real-time FPS counter overlay
- 🎨 Custom drawing utilities:
  - Text rendering with customizable colors
  - Line drawing with adjustable thickness
  - Box drawing with transparency support
- 🖥️ Window resizing and fullscreen support
- 🎯 Color management system

## 📋 Prerequisites

- Windows 10/11
- Visual Studio 2019 or newer
- DirectX 11 SDK
- Windows SDK

## 🛠️ Installation

1. Clone the repository https://github.com/GamebP/Native-D3D11.git  

2. Open `NativeD3D11.sln` in Visual Studio
3. Build the solution (F5 or Ctrl+Shift+B)

## 📁 Project Structure

NativeD3D11/  
├── Addons/  
│ ├── colors.h # Color management  
│ ├── text.h # Text rendering utilities  
│ ├── line.h # Line drawing utilities  
│ └── box.h # Box drawing utilities  
└── NativeD3D11.cpp # Main application code

## 💻 Code Examples

### Drawing Text
```cpp
// Draw text with custom color
text::draw(d2dRenderTarget, 
    CreateBrush(d2dRenderTarget, Colors::UI::TextPrimary()), 
    textFormat, L"Hello World!", 
    5.0f, 5.0f);
```

### Drawing Lines
```cpp
// Draw a line
line::put(d2dRenderTarget, 
    CreateBrush(d2dRenderTarget, Colors::Get::Red()), 
    200.0f, 150.0f,    // start point (x1, y1)
    400.0f, 150.0f,    // end point (x2, y2)
    3.0f);             // thickness
```

### Drawing Boxes
```cpp
// Draw a box with transparency
box::put(d2dRenderTarget, 
    CreateBrush(d2dRenderTarget, Colors::UI::Highlight()), 
    100, 100,          // top-left corner (x1, y1)
    300, 250,          // bottom-right corner (x2, y2)
    2.0f,              // thickness
    0.5f);             // opacity
```


## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
