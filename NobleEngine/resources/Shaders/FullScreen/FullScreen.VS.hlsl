
static const uint kNumVertex = 3;
static const float4 kPosition[kNumVertex] =
{
    float4(-1.0f, 1.0f, 0.0f, 1.0f), // 左上
    float4(3.0f, 1.0f, 0.0f, 1.0f), // 右上
    float4(-1.0f, -3.0f, 0.0f, 1.0f) // 左下
};
static const float2 kTexCoord[kNumVertex] =
{
    float2(0.0f, 0.0f), // 左上
    float2(2.0f, 0.0f), // 右上
    float2(0.0f, 2.0f) // 左下
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.position = kPosition[vertexID];
    output.texcoord = kTexCoord[vertexID];
    return output;
}