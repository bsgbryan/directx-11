#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

#include "enums/ConstantBuffers.h"

#include "structs/VertexPositionColor.h"
#include "structs/Descriptions.h"

#pragma comment(lib, "d3dcompiler.lib")

using DirectX::XMMATRIX;

using DirectX::XMConvertToRadians;
using DirectX::XMMatrixPerspectiveFovLH;

using Descriptions::Buffer;

XMMATRIX g_ProjectionMatrix;

D3D11_SUBRESOURCE_DATA CreateResourceData() {
    D3D11_SUBRESOURCE_DATA resourceData;

    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

    return resourceData;
}

D3D11_BUFFER_DESC constantBufferDesc = Buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(XMMATRIX));

template<typename I, typename V, std::size_t IS, std::size_t VS>
bool LoadContent(
    D3DContext& context,
    I (&indicies)[IS],
    V (&vertices)[VS],
    UINT byteWidth
) {
    assert(context.device);

    D3D11_BUFFER_DESC vertexBufferDesc = Buffer(D3D11_BIND_VERTEX_BUFFER, byteWidth);
    D3D11_BUFFER_DESC indexBufferDesc = Buffer(D3D11_BIND_INDEX_BUFFER, sizeof(WORD) * IS);

    D3D11_SUBRESOURCE_DATA resourceData = CreateResourceData();

    resourceData.pSysMem = vertices;

    HRESULT hr = context.device->CreateBuffer(&vertexBufferDesc, &resourceData, &context.vertexBuffer);

    if (FAILED(hr))
        return false;

    resourceData.pSysMem = indicies;

    hr = context.device->CreateBuffer(&indexBufferDesc, &resourceData, &context.indexBuffer);

    if (FAILED(hr))
        return false;

    hr = context.device->CreateBuffer(&constantBufferDesc, nullptr, &context.constantBuffers[CB_Application]);

    if (FAILED(hr))
        return false;

    hr = context.device->CreateBuffer(&constantBufferDesc, nullptr, &context.constantBuffers[CB_Frame]);

    if (FAILED(hr))
        return false;

    hr = context.device->CreateBuffer(&constantBufferDesc, nullptr, &context.constantBuffers[CB_Object]);

    if (FAILED(hr))
        return false;

    // Load the compiled vertex shader.
    ID3DBlob* vertexShaderBlob;
    LPCWSTR compiledVertexShaderObject = L"./out/SimpleVertexShader.cso";

    hr = D3DReadFileToBlob(compiledVertexShaderObject, &vertexShaderBlob);

    if (FAILED(hr))
        return false;

    hr = context.device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &context.vertexShader);

    if (FAILED(hr))
        return false;

    // Create the input layout for the vertex shader.
    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = context.device->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &context.inputLayout);
    if (FAILED(hr))
        return false;

    SafeRelease(vertexShaderBlob);

    // Load the compiled pixel shader.
    ID3DBlob* pixelShaderBlob;
    LPCWSTR compiledPixelShaderObject = L"./out/SimplePixelShader.cso";

    hr = D3DReadFileToBlob(compiledPixelShaderObject, &pixelShaderBlob);

    if (FAILED(hr))
        return false;

    hr = context.device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &context.pixelShader);

    if (FAILED(hr))
        return false;

    SafeRelease(pixelShaderBlob);

    // Setup the projection matrix.
    RECT clientRect;
    GetClientRect(g_WindowHandle, &clientRect);

    // Compute the exact client dimensions.
    // This is required for a correct projection matrix.
    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);

    context.deviceContext->UpdateSubresource(context.constantBuffers[CB_Application], 0, nullptr, &g_ProjectionMatrix, 0, 0);

    return true;
}

