#pragma once

#include <d3d11.h>

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
    UINT width = 0;
    UINT height = 0;
};
