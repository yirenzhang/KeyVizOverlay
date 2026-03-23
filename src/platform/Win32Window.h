#pragma once

#include <windows.h>

namespace keyviz
{
class Win32Window
{
public:
    using MessageHandler = LRESULT (*)(void* context, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    Win32Window() = default;
    ~Win32Window();

    bool Create(HINSTANCE instance, int showCommand);
    void Destroy();
    bool ProcessMessages();

    void SetMessageHandler(MessageHandler handler, void* context = nullptr);
    bool RegisterKeyboardRawInput();
    void MoveBy(int deltaX, int deltaY);
    void ResizeClientSize(int clientWidth, int clientHeight);
    void SetOpacity(BYTE opacity);
    void SetClickThrough(bool enabled);

    HWND GetHwnd() const;
    HINSTANCE GetInstance() const;

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void RegisterWindowClass();
    void UnregisterWindowClass();

    HINSTANCE m_instance = nullptr;
    HWND m_hwnd = nullptr;
    MessageHandler m_messageHandler = nullptr;
    void* m_messageHandlerContext = nullptr;
    bool m_classRegistered = false;
    BYTE m_opacity = 255;
    bool m_clickThrough = false;
};
}
