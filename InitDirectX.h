#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Windows.h"
#include "SafeRelease.h"

#include "enums/ConstantBuffers.h"

#include "structs/Descriptions.h"
#include "structs/D3DContext.h"

#pragma comment(lib, "d3d11.lib")

using Descriptions::Texture2D;
using Descriptions::SwapChain;
using Descriptions::DepthStencil;
using Descriptions::Rasterizer;

bool CreateDepthStencil(D3DContext& context) {
    D3D11_TEXTURE2D_DESC desc = Texture2D(context.width, context.height);

    return (
        FAILED(context.device->CreateTexture2D(&desc, nullptr, &context.depthStencilBuffer))
        ||
        FAILED(context.device->CreateDepthStencilView(context.depthStencilBuffer, nullptr, &context.depthStencilView))
    ) == false;
}

bool CreateRenderTargetView(D3DContext& context) {
    ID3D11Texture2D* backBuffer;

    bool succeeded = (
        FAILED(context.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer))
        ||
        FAILED(context.device->CreateRenderTargetView(backBuffer, nullptr, &context.renderTargetView))
    ) == false;

    SafeRelease(backBuffer);

    return succeeded;
}

bool CreateRasterizerState(D3DContext& context) {
    D3D11_RASTERIZER_DESC desc = Rasterizer();

    return FAILED(context.device->CreateRasterizerState(&desc, &context.rasterizerState)) == false;
}

bool CreateDeviceAndSwapChain(D3DContext& context) {
    RECT clientRect;

    GetClientRect(g_WindowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    context.width = clientRect.right - clientRect.left;
    context.height = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = SwapChain(context.width, context.height, context.enableVSync);

    UINT createDeviceFlags = 0;

    #if _DEBUG
        createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL featureLevel;

    return FAILED(
        D3D11CreateDeviceAndSwapChain(
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
        )
    ) == false;
}

void ConfigureViewport(D3DContext& context) {
    context.viewport.Width = static_cast<float>(context.width);
    context.viewport.Height = static_cast<float>(context.height);
    context.viewport.TopLeftX = 0.0f;
    context.viewport.TopLeftY = 0.0f;
    context.viewport.MinDepth = 0.0f;
    context.viewport.MaxDepth = 1.0f;
}

int InitDirectX(HINSTANCE hInstance, D3DContext& context) {
    // A window handle must have been created already.
    assert(g_WindowHandle != 0);

    bool succeeded =
        CreateDeviceAndSwapChain(context)
        &&
        CreateRenderTargetView(context)
        &&
        CreateDepthStencil(context)
        &&
        CreateRasterizerState(context);

    if (succeeded) {
        ConfigureViewport(context);

        return 0;
    }
    else
        return -1;

}

