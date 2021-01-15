#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Windows.h"
#include "SafeRelease.h"

#include "structs/Descriptions.h"

#pragma comment(lib, "d3d11.lib")

using Descriptions::Texture2D;
using Descriptions::SwapChain;
using Descriptions::DepthStencil;
using Descriptions::Rasterizer;

enum ConstantBuffer {
    CB_Application,
    CB_Frame,
    CB_Object
};

struct D3DContext {
    BOOL enableVSync = false;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    ID3D11InputLayout* inputLayout;
    ID3D11RasterizerState* rasterizerState;
    IDXGISwapChain* swapChain;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11DepthStencilState* depthStencilState;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11Buffer* constantBuffers[3];
    D3D11_VIEWPORT viewport = { 0 };
};

/**
 * Initialize the DirectX device and swap chain.
 */
int InitDirectX(HINSTANCE hInstance, D3DContext& context) {
    // A window handle must have been created already.
    assert(g_WindowHandle != 0);

    RECT clientRect;
    GetClientRect(g_WindowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    unsigned int clientWidth = clientRect.right - clientRect.left;
    unsigned int clientHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = SwapChain(clientWidth, clientHeight, context.enableVSync);

    UINT createDeviceFlags = 0;

#if _DEBUG
    createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    // These are the feature levels that we will accept.
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // This will be the feature level that 
    // is used to create our device and swap chain.
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &context.swapChain,
        &context.device,
        &featureLevel,
        &context.deviceContext
    );

    if (hr == E_INVALIDARG)
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            &featureLevels[1],
            _countof(featureLevels) - 1,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &context.swapChain,
            &context.device,
            &featureLevel,
            &context.deviceContext
        );

    if (FAILED(hr))
        return -1;

    // Next initialize the back buffer of the swap chain and associate it to a 
    // render target view.
    ID3D11Texture2D* backBuffer;

    hr = context.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

    if (FAILED(hr))
        return -1;

    hr = context.device->CreateRenderTargetView(backBuffer, nullptr, &context.renderTargetView);

    if (FAILED(hr))
        return -1;

    SafeRelease(backBuffer);

    // Create the depth buffer for use with the depth/stencil view.
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc = Texture2D(clientWidth, clientHeight);

    hr = context.device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &context.depthStencilBuffer);

    if (FAILED(hr))
        return -1;

    hr = context.device->CreateDepthStencilView(context.depthStencilBuffer, nullptr, &context.depthStencilView);

    if (FAILED(hr))
        return -1;

    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = DepthStencil();

    hr = context.device->CreateDepthStencilState(&depthStencilStateDesc, &context.depthStencilState);

    if (FAILED(hr))
        return -1;

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc = Rasterizer();

    // Create the rasterizer state object.
    hr = context.device->CreateRasterizerState(&rasterizerDesc, &context.rasterizerState);

    if (FAILED(hr))
        return -1;

    // Initialize the viewport to occupy the entire client area.
    context.viewport.Width = static_cast<float>(clientWidth);
    context.viewport.Height = static_cast<float>(clientHeight);
    context.viewport.TopLeftX = 0.0f;
    context.viewport.TopLeftY = 0.0f;
    context.viewport.MinDepth = 0.0f;
    context.viewport.MaxDepth = 1.0f;

    return 0;
}

