
struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

cbuffer Scale : register(b1)
{
    float scale;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    
    float4 color = textures[textureIndex].Sample(gSampler, input.TexCoord);
    float value = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    float4 grayColor = float4(value, value, value, color.a);
    output.Color = lerp(color, grayColor, scale);
    return output;
    
    
    //output.Color = textures[textureIndex].Sample(gSampler, input.TexCoord);
    //float value = dot(output.Color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    //output.Color.rgb = float3(value, value, value);
    //return output;
}