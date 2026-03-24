#include "Application.h"

#include <windows.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ui/OverlayFontAtlas.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace keyviz
{
LRESULT Application::WindowMessageThunk(void* context, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto* application = static_cast<Application*>(context);
    if (application == nullptr)
    {
        return 0;
    }

    if (msg == WM_INPUT)
    {
        application->m_inputService.HandleRawInput(reinterpret_cast<HRAWINPUT>(lParam));
        return 1;
    }

    if (!application->m_imguiWin32Initialized)
    {
        return 0;
    }

    return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
}

void Application::RequestExitThunk(void* context)
{
    (void)context;
    PostQuitMessage(0);
}

void Application::MoveWindowThunk(void* context, int deltaX, int deltaY)
{
    auto* application = static_cast<Application*>(context);
    if (application == nullptr)
    {
        return;
    }

    application->m_window.MoveBy(deltaX, deltaY);
}

void Application::WindowDragStateThunk(void* context, bool active)
{
    auto* application = static_cast<Application*>(context);
    if (application == nullptr)
    {
        return;
    }

    application->m_windowDragActive = active;
}

int Application::Run()
{
    if (!Initialize())
    {
        return 1;
    }

    __try
    {
        while (m_window.ProcessMessages())
        {
            Tick();
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        MessageBoxW(nullptr, L"Unhandled runtime exception occurred.", L"KeyVizOverlay crashed", MB_ICONERROR | MB_OK);
        Shutdown();
        return 1;
    }

    Shutdown();
    return 0;
}

bool Application::Initialize()
{
    auto failInitialize = [&](const wchar_t* reason) -> bool
    {
        MessageBoxW(nullptr, reason, L"KeyVizOverlay startup failed", MB_ICONERROR | MB_OK);
        Shutdown();
        return false;
    };

    m_inputService.Initialize();
    m_window.SetMessageHandler(&Application::WindowMessageThunk, this);

    const HINSTANCE instance = GetModuleHandleW(nullptr);
    const wchar_t* failureReason = nullptr;
    if (!InitializeWindowAndInput(instance, failureReason))
    {
        return failInitialize(failureReason != nullptr ? failureReason : L"Failed to initialize input window.");
    }
    if (!InitializeRendererAndImGui(failureReason))
    {
        return failInitialize(failureReason != nullptr ? failureReason : L"Failed to initialize renderer.");
    }

    ConfigureUiBindings();
    m_ui.Initialize();
    m_layoutResizePending = false;

    CaptureInitialClientSize();

    m_lastFrameTime = std::chrono::steady_clock::now();
    return true;
}

bool Application::InitializeWindowAndInput(HINSTANCE instance, const wchar_t*& failureReason)
{
    if (!m_window.Create(instance, SW_SHOWDEFAULT))
    {
        failureReason = L"Failed to create the overlay window.";
        return false;
    }

    ImGui_ImplWin32_EnableAlphaCompositing(m_window.GetHwnd());

    if (!m_window.RegisterKeyboardRawInput())
    {
        failureReason = L"Failed to register keyboard raw input.";
        return false;
    }

    return true;
}

bool Application::InitializeRendererAndImGui(const wchar_t*& failureReason)
{
    if (!m_renderer.Initialize(m_window.GetHwnd()))
    {
        failureReason = L"Failed to initialize the D3D11 renderer.";
        return false;
    }

    if (!InitializeImGuiContextAndStyle())
    {
        return false;
    }

    if (!InitializeImGuiBackends(failureReason))
    {
        return false;
    }

    return true;
}

bool Application::InitializeImGuiContextAndStyle()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_imguiContextCreated = true;

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    BuildOverlayFontAtlas(io);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.AntiAliasedLines = false;
    style.AntiAliasedLinesUseTex = false;
    style.AntiAliasedFill = false;
    return true;
}

bool Application::InitializeImGuiBackends(const wchar_t*& failureReason)
{
    if (!ImGui_ImplWin32_Init(m_window.GetHwnd()))
    {
        failureReason = L"Failed to initialize Dear ImGui Win32 backend.";
        return false;
    }
    m_imguiWin32Initialized = true;

    if (!ImGui_ImplDX11_Init(m_renderer.GetDevice(), m_renderer.GetDeviceContext()))
    {
        failureReason = L"Failed to initialize Dear ImGui DX11 backend.";
        return false;
    }
    m_imguiDx11Initialized = true;
    return true;
}

void Application::ConfigureUiBindings()
{
    m_ui.SetExitRequestHandler(&Application::RequestExitThunk, this);
    m_ui.SetDragRequestHandler(&Application::MoveWindowThunk, this);
    m_ui.SetDragStateRequestHandler(&Application::WindowDragStateThunk, this);
}

void Application::CaptureInitialClientSize()
{
    RECT clientRect{};
    if (GetClientRect(m_window.GetHwnd(), &clientRect))
    {
        m_lastClientWidth = static_cast<unsigned int>(clientRect.right - clientRect.left);
        m_lastClientHeight = static_cast<unsigned int>(clientRect.bottom - clientRect.top);
    }
}

void Application::ShutdownImGui()
{
    if (m_imguiDx11Initialized)
    {
        ImGui_ImplDX11_Shutdown();
        m_imguiDx11Initialized = false;
    }

    if (m_imguiWin32Initialized)
    {
        ImGui_ImplWin32_Shutdown();
        m_imguiWin32Initialized = false;
    }

    if (m_imguiContextCreated)
    {
        ImGui::DestroyContext();
        m_imguiContextCreated = false;
    }
}

void Application::Shutdown()
{
    m_ui.Shutdown();
    ShutdownImGui();

    m_renderer.Shutdown();
    m_inputService.Shutdown();
    m_window.Destroy();
}

void Application::CollectPreferredClientSize()
{
    const ImVec2 preferredSize = m_ui.GetPreferredWindowSize();
    if (preferredSize.x <= 0.0f || preferredSize.y <= 0.0f)
    {
        return;
    }

    const int targetWidth = static_cast<int>(preferredSize.x + 0.5f);
    const int targetHeight = static_cast<int>(preferredSize.y + 0.5f);
    if (targetWidth <= 0 || targetHeight <= 0)
    {
        return;
    }

    if (targetWidth == m_pendingClientWidth && targetHeight == m_pendingClientHeight)
    {
        return;
    }

    m_pendingClientWidth = targetWidth;
    m_pendingClientHeight = targetHeight;
    m_layoutResizePending = true;
}

bool Application::ApplyPendingResize()
{
    if (!m_layoutResizePending)
    {
        return true;
    }

    // 拖动滑条期间暂缓重设宿主窗口，避免频繁 Resize 导致交互卡顿或窗口状态异常。
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        return false;
    }

    if (m_pendingClientWidth > 0 && m_pendingClientHeight > 0)
    {
        m_window.ResizeClientSize(m_pendingClientWidth, m_pendingClientHeight);
    }
    m_layoutResizePending = false;
    return true;
}

void Application::UpdateOverlayState(float deltaSeconds)
{
    m_inputService.Update(deltaSeconds);
    m_ui.Update(deltaSeconds, m_inputService);
    m_window.SetClickThrough(m_ui.IsConsoleHidden());
    m_windowDragActive = false;
}

void Application::BeginImGuiFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Application::RenderUiFrame()
{
    m_ui.Render(m_inputService);
    ImGui::Render();
}

void Application::PresentFrame()
{
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_renderer.BeginFrame(clearColor);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    m_renderer.EndFrame();
    m_inputService.ClearFrameFlags();
}

void Application::Tick()
{
    const auto now = std::chrono::steady_clock::now();
    const float deltaSeconds = std::chrono::duration<float>(now - m_lastFrameTime).count();
    m_lastFrameTime = now;

    UpdateOverlayState(deltaSeconds);
    BeginImGuiFrame();
    RenderUiFrame();
    PresentFrame();
    UpdateRendererSize();
}

void Application::UpdateRendererSize()
{
    if (!m_renderer.IsReady() || m_windowDragActive)
    {
        return;
    }

    CollectPreferredClientSize();
    if (!ApplyPendingResize())
    {
        return;
    }

    RECT clientRect{};
    if (!GetClientRect(m_window.GetHwnd(), &clientRect))
    {
        return;
    }

    const unsigned int clientWidth = static_cast<unsigned int>(clientRect.right - clientRect.left);
    const unsigned int clientHeight = static_cast<unsigned int>(clientRect.bottom - clientRect.top);
    if (clientWidth == 0U || clientHeight == 0U)
    {
        return;
    }

    if (clientWidth == m_lastClientWidth && clientHeight == m_lastClientHeight)
    {
        return;
    }

    m_lastClientWidth = clientWidth;
    m_lastClientHeight = clientHeight;
    m_renderer.Resize(clientWidth, clientHeight);
}
} // namespace keyviz
