#pragma once

#include <algorithm>

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "InitDirectX.h"

#pragma comment(lib, "winmm.lib")

using namespace DirectX;

XMMATRIX g_WorldMatrix;
XMMATRIX g_ViewMatrix;

void Update(D3DContext& context, float deltaTime) {
    XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
    XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);

    g_ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
    context.deviceContext->UpdateSubresource(context.constantBuffers[CB_Frame], 0, nullptr, &g_ViewMatrix, 0, 0);


    static float angle = 0.0f;
    angle += 90.0f * deltaTime;
    XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);

    g_WorldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
    context.deviceContext->UpdateSubresource(context.constantBuffers[CB_Object], 0, nullptr, &g_WorldMatrix, 0, 0);
}

void Clear(D3DContext& context, const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil) {
    context.deviceContext->ClearRenderTargetView(context.renderTargetView, clearColor);
    context.deviceContext->ClearDepthStencilView(context.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Render(D3DContext& context, UINT vertexStride, UINT indexCount) {
    assert(context.device);
    assert(context.deviceContext);

    Clear(context, Colors::CornflowerBlue, 1.0f, 0);

    const UINT offset = 0;

    context.deviceContext->IASetVertexBuffers(0, 1, &context.vertexBuffer, &vertexStride, &offset);
    context.deviceContext->IASetInputLayout(context.inputLayout);
    context.deviceContext->IASetIndexBuffer(context.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context.deviceContext->VSSetShader(context.vertexShader, nullptr, 0);
    context.deviceContext->VSSetConstantBuffers(0, 3, context.constantBuffers);

    context.deviceContext->RSSetState(context.rasterizerState);
    context.deviceContext->RSSetViewports(1, &context.viewport);

    context.deviceContext->PSSetShader(context.pixelShader, nullptr, 0);

    context.deviceContext->OMSetRenderTargets(1, &context.renderTargetView, context.depthStencilView);
    context.deviceContext->OMSetDepthStencilState(context.depthStencilState, 1);

    context.deviceContext->DrawIndexed(indexCount, 0, 0);

    context.swapChain->Present(context.enableVSync ? 1 : 0, 0);
}

int Run(D3DContext& context, UINT byteWidth, UINT vertexCount) {
    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();
    static const float targetFramerate = 30.0f;
    static const float maxTimeStep = 1.0f / targetFramerate;

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            DWORD currentTime = timeGetTime();
            float deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;

            // Cap the delta time to the max time step (useful if your 
            // debugging and you don't want the deltaTime value to explode.
            deltaTime = std::min<float>(deltaTime, maxTimeStep);

            Update(context, deltaTime);
            Render(context, byteWidth, vertexCount);
        }
    }

    return static_cast<int>(msg.wParam);
}
