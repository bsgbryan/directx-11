#pragma once

#include <d3d11.h>

using DirectX::XMFLOAT3;

// Vertex data for a colored cube.
struct VertexPosColor {
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};
