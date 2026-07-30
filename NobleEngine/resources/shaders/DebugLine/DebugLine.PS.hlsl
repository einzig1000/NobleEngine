// DebugLine.PS.hlsl
// デバッグライン用。単色を塗るだけ

struct PSInput
{
    float4 position : SV_POSITION;
};

cbuffer ColorBuffer : register(b0)
{
    float4 color;
}

float4 main(PSInput input) : SV_TARGET
{
    return color;
}
