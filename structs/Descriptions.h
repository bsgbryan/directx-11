#pragma once

#include <d3d11.h>

#include "QueryRefreshRate.h"

namespace Descriptions {
    D3D11_BUFFER_DESC Buffer(D3D11_BIND_FLAG flag, UINT byteWidth) {
        D3D11_BUFFER_DESC desc;

        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

        desc.BindFlags = flag;
        desc.ByteWidth = byteWidth;
        desc.CPUAccessFlags = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;

        return desc;
    }

    D3D11_TEXTURE2D_DESC Texture2D(UINT width, UINT height) {
        D3D11_TEXTURE2D_DESC desc;

        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        desc.CPUAccessFlags = 0;
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;

        return desc;
    }

    DXGI_SWAP_CHAIN_DESC SwapChain(UINT width, UINT height, BOOL enableVSync) {
        DXGI_SWAP_CHAIN_DESC desc;

        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

        desc.BufferCount = 1;
        desc.BufferDesc.Width = width;
        desc.BufferDesc.Height = height;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.RefreshRate = QueryRefreshRate(width, height, enableVSync);
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.OutputWindow = g_WindowHandle;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        desc.Windowed = TRUE;

        return desc;
    }

    D3D11_DEPTH_STENCIL_DESC DepthStencil() {
        D3D11_DEPTH_STENCIL_DESC desc;

        ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = FALSE;

        return desc;
    }

    D3D11_RASTERIZER_DESC Rasterizer() {
        D3D11_RASTERIZER_DESC desc;

        ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

        desc.AntialiasedLineEnable = FALSE;
        desc.CullMode = D3D11_CULL_BACK;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0.0f;
        desc.DepthClipEnable = TRUE;
        desc.FillMode = D3D11_FILL_SOLID;
        desc.FrontCounterClockwise = FALSE;
        desc.MultisampleEnable = FALSE;
        desc.ScissorEnable = FALSE;
        desc.SlopeScaledDepthBias = 0.0f;

        return desc;
    }
}
