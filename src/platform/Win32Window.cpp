#include "Win32Window.h"

#include "util/Constants.h"

namespace keyviz
{
Win32Window::~Win32Window()
{
    Destroy();
}

bool Win32Window::Create(HINSTANCE instance, int showCommand)
{
    m_instance = instance;
    RegisterWindowClass();

    m_hwnd = CreateWindowExW(
        kWindowExStyle,
        kWindowClassName,
        kWindowTitle,
        kWindowStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        kDefaultWindowWidth,
        kDefaultWindowHeight,
        nullptr,
        nullptr,
        m_instance,
        this);

    if (m_hwnd == nullptr)
    {
        return false;
    }

    SetOpacity(kWindowOpacity);
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    ShowWindow(m_hwnd, showCommand);
    UpdateWindow(m_hwnd);
    return true;
}

void Win32Window::Destroy()
{
    if (m_hwnd != nullptr)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    UnregisterWindowClass();
    m_instance = nullptr;
}

bool Win32Window::ProcessMessages()
{
    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT)
        {
            return false;
        }
    }

    return true;
}

void Win32Window::SetMessageHandler(MessageHandler handler, void* context)
{
    m_messageHandler = handler;
    m_messageHandlerContext = context;
}

bool Win32Window::RegisterKeyboardRawInput()
{
    if (m_hwnd == nullptr)
    {
        return false;
    }

    RAWINPUTDEVICE device{};
    device.usUsagePage = 0x01;
    device.usUsage = 0x06;
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = m_hwnd;
    return RegisterRawInputDevices(&device, 1, sizeof(device)) == TRUE;
}

void Win32Window::MoveBy(int deltaX, int deltaY)
{
    if (m_hwnd == nullptr || (deltaX == 0 && deltaY == 0))
    {
        return;
    }

    RECT windowRect{};
    if (!GetWindowRect(m_hwnd, &windowRect))
    {
        return;
    }

    SetWindowPos(
        m_hwnd,
        nullptr,
        windowRect.left + deltaX,
        windowRect.top + deltaY,
        0,
        0,
        SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOREDRAW | SWP_NOCOPYBITS);
}

void Win32Window::ResizeClientSize(int clientWidth, int clientHeight)
{
    if (m_hwnd == nullptr || clientWidth <= 0 || clientHeight <= 0)
    {
        return;
    }

    RECT currentClientRect{};
    if (GetClientRect(m_hwnd, &currentClientRect))
    {
        const int currentWidth = currentClientRect.right - currentClientRect.left;
        const int currentHeight = currentClientRect.bottom - currentClientRect.top;
        if (currentWidth == clientWidth && currentHeight == clientHeight)
        {
            return;
        }
    }

    RECT desiredRect{ 0, 0, clientWidth, clientHeight };
    if (!AdjustWindowRectEx(&desiredRect, kWindowStyle, FALSE, kWindowExStyle))
    {
        return;
    }

    RECT currentWindowRect{};
    if (!GetWindowRect(m_hwnd, &currentWindowRect))
    {
        return;
    }

    const int windowWidth = desiredRect.right - desiredRect.left;
    const int windowHeight = desiredRect.bottom - desiredRect.top;
    SetWindowPos(
        m_hwnd,
        nullptr,
        currentWindowRect.left,
        currentWindowRect.top,
        windowWidth,
        windowHeight,
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOREDRAW | SWP_NOCOPYBITS);
}

void Win32Window::SetOpacity(BYTE opacity)
{
    m_opacity = opacity;

    if (m_hwnd == nullptr)
    {
        return;
    }

    SetLayeredWindowAttributes(m_hwnd, 0, m_opacity, LWA_ALPHA);
}


HWND Win32Window::GetHwnd() const
{
    return m_hwnd;
}

HINSTANCE Win32Window::GetInstance() const
{
    return m_instance;
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* window = nullptr;
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        window = static_cast<Win32Window*>(createStruct->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->m_hwnd = hwnd;
    }
    else
    {
        window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (window != nullptr)
    {
        return window->HandleMessage(hwnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT Win32Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (m_messageHandler != nullptr)
    {
        const LRESULT handled = m_messageHandler(m_messageHandlerContext, hwnd, msg, wParam, lParam);
        if (handled != 0)
        {
            return handled;
        }
    }

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    default:
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void Win32Window::RegisterWindowClass()
{
    if (m_classRegistered)
    {
        return;
    }

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = m_instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kWindowClassName;

    RegisterClassExW(&windowClass);
    m_classRegistered = true;
}

void Win32Window::UnregisterWindowClass()
{
    if (!m_classRegistered || m_instance == nullptr)
    {
        return;
    }

    UnregisterClassW(kWindowClassName, m_instance);
    m_classRegistered = false;
}
}
