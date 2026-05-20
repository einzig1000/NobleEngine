
struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    output.Color = gTexture.Sample(gSampler, input.TexCoord);
    
    // 周囲を0に、中心になるほど明るくする
    float2 correct = input.TexCoord * (1.0f - input.TexCoord.yx);
    // corretだけだと中心でも0.0625で暗すぎるので16倍する
    float1 vignette = correct.x * correct.y * 16.0f;
    vignette = saturate(pow(vignette, 0.8f));
    output.Color.rgb *= vignette;
    
    return output;
}