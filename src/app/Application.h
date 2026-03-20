#pragma once

#include <chrono>

#include "input/InputService.h"
#include "platform/Win32Window.h"
#include "rendering/D3D11Renderer.h"
#include "ui/OverlayUI.h"

namespace keyviz
{
class Application
{
public:
    Application() = default;

    int Run();

private:
    static LRESULT WindowMessageThunk(void* context, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void RequestExitThunk(void* context);
    static void MoveWindowThunk(void* context, int deltaX, int deltaY);
    static void WindowDragStateThunk(void* context, bool active);


    bool Initialize();
    void Shutdown();
    void Tick();
    void UpdateRendererSize();


    Win32Window m_window;
    D3D11Renderer m_renderer;
    OverlayUI m_ui;
    InputService m_inputService;
    std::chrono::steady_clock::time_point m_lastFrameTime{};
    unsigned int m_lastClientWidth = 0;
    unsigned int m_lastClientHeight = 0;
    bool m_windowDragActive = false;
    bool m_layoutResizePending = true;

    bool m_imguiContextCreated = false;
    bool m_imguiWin32Initialized = false;
    bool m_imguiDx11Initialized = false;
};
}
