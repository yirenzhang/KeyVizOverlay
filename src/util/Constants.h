#pragma once

#include <windows.h>

namespace keyviz
{
inline constexpr const wchar_t* kWindowClassName = L"KeyVizOverlayWindow";
inline constexpr const wchar_t* kWindowTitle = L"KeyVizOverlay";
inline constexpr int kDefaultWindowWidth = 440;
inline constexpr int kDefaultWindowHeight = 260;
inline constexpr DWORD kWindowStyle = WS_POPUP;
inline constexpr DWORD kWindowExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;
inline constexpr BYTE kWindowOpacity = 255;
}
