#pragma once

#include "InitDirectX.h"
#include "SafeRelease.h"

void UnloadContent(D3DContext& context) {
    SafeRelease(context.constantBuffers[CB_Application]);
    SafeRelease(context.constantBuffers[CB_Frame]);
    SafeRelease(context.constantBuffers[CB_Object]);
    SafeRelease(context.indexBuffer);
    SafeRelease(context.vertexBuffer);
    SafeRelease(context.inputLayout);
    SafeRelease(context.vertexShader);
    SafeRelease(context.pixelShader);
}
