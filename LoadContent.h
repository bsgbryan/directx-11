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

template<typename A, std::size_t AS>
D3D11_SUBRESOURCE_DATA CreateResourceData(A(&arr)[AS]) {
    D3D11_SUBRESOURCE_DATA data;

    ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

    data.pSysMem = arr;

    return data;
}

template<typename V, std::size_t VS>
bool CreateVertexBuffer(D3DContext& context, V(&vertices)[VS], UINT byteWidth) {
    D3D11_BUFFER_DESC      desc = Buffer(D3D11_BIND_VERTEX_BUFFER, byteWidth);
    D3D11_SUBRESOURCE_DATA data = CreateResourceData(vertices);

    return FAILED(context.device->CreateBuffer(&desc, &data, &context.vertexBuffer)) == false;
}

template<typename I, std::size_t IS>
bool CreateIndexBuffer(D3DContext& context, I(&indicies)[IS]) {
    D3D11_BUFFER_DESC      desc = Buffer(D3D11_BIND_INDEX_BUFFER, sizeof(WORD) * IS);
    D3D11_SUBRESOURCE_DATA data = CreateResourceData(indicies);

    return FAILED(context.device->CreateBuffer(&desc, &data, &context.indexBuffer)) == false;
}

bool CreateConstantBuffers(D3DContext& context) {
    D3D11_BUFFER_DESC constantBufferDesc = Buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(XMMATRIX));

    return
        FAILED(context.device->CreateBuffer(&constantBufferDesc, nullptr, &context.constantBuffers[CB_Application])) == false
        &&
        FAILED(context.device->CreateBuffer(&constantBufferDesc, nullptr, &context.constantBuffers[CB_Frame])) == false
        &&
        FAILED(context.device->CreateBuffer(&constantBufferDesc, nullptr, &context.constantBuffers[CB_Object])) == false;
}

bool CreateVertexShader(LPCWSTR pathToCompiledObject, D3DContext& context) {
    ID3DBlob* vertexShaderBlob;

    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    bool succeeded =
        FAILED(D3DReadFileToBlob(pathToCompiledObject, &vertexShaderBlob)) == false
        &&
        FAILED(context.device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &context.vertexShader)) == false
        &&
        FAILED(context.device->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &context.inputLayout)) == false;

    SafeRelease(vertexShaderBlob);

    return succeeded;
}

bool CreatePixelShader(LPCWSTR pathToCompiledObject, D3DContext& context) {
    ID3DBlob* pixelShaderBlob;

    bool succeeded =
        FAILED(D3DReadFileToBlob(pathToCompiledObject, &pixelShaderBlob)) == false
        &&
        FAILED(context.device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &context.pixelShader)) == false;

    SafeRelease(pixelShaderBlob);

    return succeeded;
}

void ConfigureAndSetProjectionMatrix(D3DContext& context) {
    RECT clientRect;
    GetClientRect(g_WindowHandle, &clientRect);

    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    XMMATRIX g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);

    context.deviceContext->UpdateSubresource(context.constantBuffers[CB_Application], 0, nullptr, &g_ProjectionMatrix, 0, 0);
}

template<typename I, typename V, std::size_t IS, std::size_t VS>
bool LoadContent(D3DContext& context, I (&indicies)[IS], V (&vertices)[VS], UINT byteWidth) {
    assert(context.device);

    bool succeeded =
        CreateVertexBuffer(context, vertices, byteWidth)
        &&
        CreateIndexBuffer(context, indicies)
        &&
        CreateConstantBuffers(context)
        &&
        CreateVertexShader(TEXT("./out/SimpleVertexShader.cso"), context)
        &&
        CreatePixelShader(TEXT("./out/SimplePixelShader.cso"), context);

    if (succeeded) {
        ConfigureAndSetProjectionMatrix(context);

        return true;
    }
    else
        return false;
}

