#pragma once

#include <windows.h>
#include <d3d11.h>

namespace keyviz
{
class D3D11Renderer
{
public:
    D3D11Renderer() = default;
    ~D3D11Renderer();

    bool Initialize(HWND hwnd);
    void Shutdown();
    void Resize(unsigned int width, unsigned int height);
    void BeginFrame(const float clearColor[4]);
    void EndFrame();

    bool IsReady() const;
    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

private:
    bool CreateDevice(HWND hwnd);
    void CreateRenderTarget();
    void DestroyRenderTarget();

    HWND m_hwnd = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    bool m_initialized = false;
};
}