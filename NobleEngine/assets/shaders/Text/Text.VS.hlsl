struct GlyphInstance
{
    float2 position;
    float2 size;
    float2 uvMin;
    float2 uvMax;
    float4 color;
};

StructuredBuffer<GlyphInstance> gGlyphs : register(t0);

cbuffer VSConstants : register(b0)
{
    float2 targetSize;
    float2 pad0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

static const float2 kCorner[4] =
{
    float2(1.0f, 1.0f), // index0: 右下(BR)
    float2(0.0f, 0.0f), // index1: 左上(TL)
    float2(0.0f, 1.0f), // index2: 左下(BL)
    float2(1.0f, 0.0f), // index3: 右上(TR)
};

VSOutput main(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    GlyphInstance inst = gGlyphs[instanceID];
    float2 corner = kCorner[vertexID % 4];

    float2 pixelPos = inst.position + corner * inst.size;
    float2 ndc = (pixelPos / targetSize) * 2.0f - 1.0f;
    ndc.y = -ndc.y;

    VSOutput output;
    output.position = float4(ndc, 0.0f, 1.0f);
    output.texcoord = lerp(inst.uvMin, inst.uvMax, corner);
    output.color = inst.color;
    return output;
}