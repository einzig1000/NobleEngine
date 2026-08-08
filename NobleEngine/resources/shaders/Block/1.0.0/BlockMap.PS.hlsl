// BlockMap.PS.hlsl

#include "BlockMap.hlsli"

// サンプラーのレジスタ番号を s0 に設定
SamplerState gSampler : register(s0);
// テクスチャのレジスタ番号を t0 に設定
Texture2D<float32_t4> gTexture0 : register(t0);
// Material: レジスタ b0 に設定
ConstantBuffer<Material> gMaterial0 : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    // UV同次座標
    PixelShaderOutput output;
    
    // 1. テクスチャ座標の変換
    float32_t4 transformedUV = mul(gMaterial0.uvTransform, float32_t4(input.texcoord, 0.0f, 1.0f));

    // 2. テクスチャから色をサンプリング
    float32_t4 textureColor = gTexture0.Sample(gSampler, transformedUV.xy);
   
    // 3. マテリアルの色とテクスチャの色を乗算
    float32_t4 finalColor = gMaterial0.color * textureColor;

    output.color = finalColor;

    if (output.color.a <= 1e-5) discard;

    return output;
}