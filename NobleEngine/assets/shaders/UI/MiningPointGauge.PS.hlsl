struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

static const int kMaxWaveCount = 8;

cbuffer GaugeParams : register(b0)
{
    float4 fillColor; // 溜まった部分の色
    float4 waveColor; // 波の色(aが強さ)
    int textureIndex; // ゲージのテクスチャ
    float fillRatio; // 溜まっている割合(0〜1)
    float waveWidth; // 波の尾の長さ(0〜1)
    float padding; // 16バイトにそろえるための詰め物
    // 波の先頭の位置(0〜1)。負なら波なし
    // cbufferの配列は1要素ごとに16バイト使うので、float[8]ではなくfloat4[2]に4個ずつ詰める
    float4 waveHeads[2];
};

SamplerState gSamplerPoint : register(s1);
static const float kArtScale = 4.0f;

PSOutput main(PSInput input)
{
    Texture2D<float4> gaugeTexture = ResourceDescriptorHeap[textureIndex];

    PSOutput output;
    output.color = gaugeTexture.Sample(gSamplerPoint, input.texCoord);

    // 横位置をドット絵の1px単位にそろえる
    uint width, height;
    gaugeTexture.GetDimensions(width, height);
    float artWidth = width / kArtScale;
    float x = (floor(input.texCoord.x * artWidth) + 0.5f) / artWidth;

    // 半透明部分だけ塗る。枠と区切り線はそのまま
    bool isCell = output.color.a > 0.0f && output.color.a < 1.0f;
    if (isCell && x < fillRatio)
    {
        output.color = fillColor;

        // 波: 先頭が一番明るく、後ろほど薄くなる。重なったところは一番明るい波を使う
        float wave = 0.0f;
        for (int i = 0; i < kMaxWaveCount; ++i)
        {
            float behind = waveHeads[i / 4][i % 4] - x;
            if (behind >= 0.0f)
            {
                wave = max(wave, 1.0f - saturate(behind / waveWidth));
            }
        }
        output.color.rgb = lerp(output.color.rgb, waveColor.rgb, wave * waveColor.a);
    }

    if (output.color.a < 0.01f)
    {
        discard;
    }
    return output;
}