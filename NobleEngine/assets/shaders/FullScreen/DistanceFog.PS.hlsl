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

cbuffer DepthTextureIndex : register(b1)
{
    int depthTextureIndex;
};

cbuffer ProjectionInverse : register(b2)
{
    float4x4 projectionInverse;
};

cbuffer FogParam : register(b3)
{
    float3 fogColor;
    float fogDensity; // >0でexp2フォグ、0で線形フォグ
    float fogStart; // 線形フォグ用
    float fogEnd; // 線形フォグ用
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 color = textures[textureIndex].Sample(gSampler, input.TexCoord);

    float ndcDepth = textures[depthTextureIndex].Sample(gSamplerPoint, input.TexCoord).r;

    // 何も描かれていない(空)ピクセルはそのまま返す
    if (ndcDepth >= 1.0f)
    {
        output.Color = color;
        return output;
    }

    // NDC → ビュー空間位置を復元(DepthBasedOutlineと同じ行ベクトル規約)
    float2 ndc = float2(input.TexCoord.x * 2.0f - 1.0f, 1.0f - input.TexCoord.y * 2.0f);
    float4 viewSpace = mul(float4(ndc, ndcDepth, 1.0f), projectionInverse);
    float3 viewPos = viewSpace.xyz * rcp(viewSpace.w);

    // カメラからの実距離(放射状フォグ。カメラを振っても濃さが変わらない)
    float dist = length(viewPos);

    float fogFactor;
    if (fogDensity > 0.0f)
    {
        // exp2フォグ:一番自然に見える
        float d = fogDensity * dist;
        fogFactor = 1.0f - exp(-d * d);
    }
    else
    {
        // 線形フォグ:チャンク描画距離にピッタリ合わせたい時
        fogFactor = saturate((dist - fogStart) * rcp(max(fogEnd - fogStart, 0.0001f)));
    }

    output.Color.rgb = lerp(color.rgb, fogColor, fogFactor);
    output.Color.a = color.a;

    return output;
}