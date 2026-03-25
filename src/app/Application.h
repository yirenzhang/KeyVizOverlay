#pragma once

#include <chrono>
#include <windows.h>

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
    bool InitializeWindowAndInput(HINSTANCE instance, const wchar_t*& failureReason);
    bool InitializeRendererAndImGui(const wchar_t*& failureReason);
    bool InitializeImGuiContextAndStyle();
    bool InitializeImGuiBackends(const wchar_t*& failureReason);
    void ConfigureUiBindings();
    void CaptureInitialClientSize();
    void ShutdownImGui();
    void CollectPreferredClientSize();
    bool ApplyPendingResize();
    void UpdateOverlayState(float deltaSeconds);
    void BeginImGuiFrame();
    void RenderUiFrame();
    void PresentFrame();
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
    POINT m_lastDragCursorPos{};
    bool m_layoutResizePending = true;
    int m_pendingClientWidth = 0;
    int m_pendingClientHeight = 0;

    bool m_imguiContextCreated = false;
    bool m_imguiWin32Initialized = false;
    bool m_imguiDx11Initialized = false;
};
}
