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

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{
    GlyphInstance inst = gGlyphs[instanceID];
    
    float2 corner = input.texcoord;

    float2 pixelPos = inst.position + corner * inst.size;
    float2 ndc = (pixelPos / targetSize) * 2.0f - 1.0f;
    ndc.y = -ndc.y;

    VSOutput output;
    output.position = float4(ndc, 0.0f, 1.0f);
    output.texcoord = lerp(inst.uvMin, inst.uvMax, corner);
    output.color = inst.color;
    return output;
}