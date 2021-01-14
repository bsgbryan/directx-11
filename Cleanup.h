#pragma once

#include "InitDirectX.h"
#include "SafeRelease.h"

void Cleanup(D3DContext& context) {
    SafeRelease(context.depthStencilView);
    SafeRelease(context.renderTargetView);
    SafeRelease(context.depthStencilBuffer);
    SafeRelease(context.depthStencilState);
    SafeRelease(context.rasterizerState);
    SafeRelease(context.swapChain);
    SafeRelease(context.deviceContext);
    SafeRelease(context.device);
}
