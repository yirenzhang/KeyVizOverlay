#include "D3D11Renderer.h"

namespace keyviz
{
D3D11Renderer::~D3D11Renderer()
{
    Shutdown();
}

bool D3D11Renderer::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;
    if (!CreateDevice(hwnd))
    {
        Shutdown();
        return false;
    }

    m_initialized = true;
    return true;
}

void D3D11Renderer::Shutdown()
{
    DestroyRenderTarget();

    if (m_swapChain != nullptr)
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

    if (m_context != nullptr)
    {
        m_context->ClearState();
        m_context->Release();
        m_context = nullptr;
    }

    if (m_device != nullptr)
    {
        m_device->Release();
        m_device = nullptr;
    }

    m_hwnd = nullptr;
    m_initialized = false;
}

void D3D11Renderer::Resize(unsigned int width, unsigned int height)
{
    if (m_swapChain == nullptr || width == 0U || height == 0U)
    {
        return;
    }

    DestroyRenderTarget();

    if (FAILED(m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
    {
        return;
    }

    CreateRenderTarget();
}

void D3D11Renderer::BeginFrame(const float clearColor[4])
{
    if (m_context == nullptr || m_renderTargetView == nullptr)
    {
        return;
    }

    RECT clientRect{};
    if (m_hwnd != nullptr && GetClientRect(m_hwnd, &clientRect))
    {
        const float width = static_cast<float>(clientRect.right - clientRect.left);
        const float height = static_cast<float>(clientRect.bottom - clientRect.top);
        if (width > 0.0f && height > 0.0f)
        {
            D3D11_VIEWPORT viewport{};
            viewport.Width = width;
            viewport.Height = height;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            m_context->RSSetViewports(1, &viewport);
        }
    }

    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    m_context->ClearRenderTargetView(m_renderTargetView, clearColor);
}

void D3D11Renderer::EndFrame()
{
    if (m_swapChain != nullptr)
    {
        m_swapChain->Present(1, 0);
    }
}

bool D3D11Renderer::IsReady() const
{
    return m_initialized;
}

ID3D11Device* D3D11Renderer::GetDevice() const
{
    return m_device;
}

ID3D11DeviceContext* D3D11Renderer::GetDeviceContext() const
{
    return m_context;
}

bool D3D11Renderer::CreateDevice(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    UINT deviceFlags = 0;

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        featureLevels,
        2u,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        &featureLevel,
        &m_context);

    if (result == DXGI_ERROR_UNSUPPORTED)
    {
        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            deviceFlags,
            featureLevels,
            2u,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &m_swapChain,
            &m_device,
            &featureLevel,
            &m_context);
    }

    if (FAILED(result))
    {
        return false;
    }

    (void)featureLevel;
    CreateRenderTarget();
    return m_renderTargetView != nullptr;
}

void D3D11Renderer::CreateRenderTarget()
{
    if (m_swapChain == nullptr || m_device == nullptr)
    {
        return;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        return;
    }

    if (FAILED(m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView)))
    {
        backBuffer->Release();
        return;
    }

    backBuffer->Release();
}

void D3D11Renderer::DestroyRenderTarget()
{
    if (m_renderTargetView != nullptr)
    {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
}
}