// WaterSurface.PS.hlsl
// 2色の液体を対流させながら混合し、ライティングして出力する。
#include "WaterSurface.hlsli"

cbuffer CameraBuffer : register(b0)
{
    float3 gCameraPositionWS;
    float gCameraPadding;
}

cbuffer ColorBuffer : register(b1)
{
    float4 gColorA; // リニア色で渡すこと
    float4 gColorB;
    float4 gBaseColor; // .a のみ最終アルファに使用

    float gColorBalance; // 0〜1。色境界の位置
    float gColorBlendWidth; // 境界の滑らかさ
    float gColorDistortion; // 境界へ加えるノイズ量
    float gColorPatternScale; // 色分布の空間スケール

    float gConvectionSpeed; // 対流の時間速度
    float gConvectionStrength; // 色分布を歪ませる強さ
    float gConvectionScale; // 対流計算に入れる座標のスケール
    float gMixProgress; // 0=分離 0.5=混ざりかけ 1=完全混合

    float gMotionIntensity;
    float gMotionWaveTime;
    float gSmoothness; // スペキュラの鋭さ
    float gFresnelStrength; // 0にすると元Unity版と同じ挙動になる
}

cbuffer LightingBuffer : register(b2)
{
    float4 gMainLightDirection; // 面→ライト方向。正規化済みで渡すこと
    float4 gMainLightColor; // rgb = 色×強度
    float4 gAmbientSky; // 上方向の環境光
    float4 gAmbientGround; // 下方向の環境光
}

// 色そのものではなく「色をサンプルする座標」を動かす。
// これにより MixProgress=0 でも A/B の領域は流れ続ける。
float2 CalculateConvectionPosition(float2 waveXZ)
{
    float time = gMotionWaveTime * gConvectionSpeed;
    float2 position = waveXZ * gConvectionScale;

    // 異なる周期を重ねて単周期運動を避けたゆっくりした回転
    float angle = sin(time * 0.31f) * 0.75f;
    angle += sin(time * 0.13f + 1.7f) * 0.35f;

    float sinAngle = sin(angle);
    float cosAngle = cos(angle);

    float2 rotatedPosition = float2(
        position.x * cosAngle - position.y * sinAngle,
        position.x * sinAngle + position.y * cosAngle);

    float flow1X = sin(rotatedPosition.y * 1.37f + time * 0.47f);
    float flow1Y = cos(rotatedPosition.x * 1.21f - time * 0.41f);

    float flow2X = cos(rotatedPosition.x * 0.83f - rotatedPosition.y * 1.91f - time * 0.29f);
    float flow2Y = sin(rotatedPosition.y * 0.97f + rotatedPosition.x * 1.73f + time * 0.33f);

    float flow3X = sin((rotatedPosition.x + rotatedPosition.y) * 0.71f + time * 0.19f);
    float flow3Y = cos((rotatedPosition.x - rotatedPosition.y) * 0.67f - time * 0.23f);

    float2 flow = float2(flow1X, flow1Y) * 0.50f
                + float2(flow2X, flow2Y) * 0.30f
                + float2(flow3X, flow3Y) * 0.20f;

    // 同じ場所でぐるぐる回るだけにならないよう、ゆっくり大きく往復させる
    float2 drift = float2(
        sin(time * 0.23f) + sin(time * 0.071f + 2.1f) * 0.35f,
        cos(time * 0.19f) + sin(time * 0.083f + 0.8f) * 0.35f);

    // 静止時も対流し、動いている時に最大+35%される
    float convectionStrength = gConvectionStrength * (1.0f + gMotionIntensity * 0.35f);

    return rotatedPosition
         + flow * convectionStrength
         + drift * convectionStrength * 0.65f;
}

// A/B境界を細かく歪ませるプロシージャルノイズ
float CalculateColorNoise(float2 colorPosition)
{
    float time = gMotionWaveTime * gConvectionSpeed;

    float noise1 = sin(colorPosition.y * 2.73f + time * 0.37f);
    float noise2 = sin(colorPosition.x * 1.91f + colorPosition.y * 1.37f - time * 0.31f);
    float noise3 = cos(colorPosition.x * 3.47f - colorPosition.y * 2.11f + time * 0.21f);
    float noise4 = sin((colorPosition.x + colorPosition.y) * 4.13f - time * 0.17f);

    return noise1 * 0.40f + noise2 * 0.30f + noise3 * 0.20f + noise4 * 0.10f;
}

// 混ざり途中にだけ見せる色ムラ用ノイズ。0〜1程度に正規化して返す
float CalculateResidualNoise(float2 colorPosition)
{
    float time = gMotionWaveTime * gConvectionSpeed;

    float noise1 = sin(colorPosition.x * 5.31f + colorPosition.y * 2.17f + time * 0.43f);
    float noise2 = cos(colorPosition.x * 2.59f - colorPosition.y * 4.73f - time * 0.37f);
    float noise3 = sin((colorPosition.x - colorPosition.y) * 6.17f + time * 0.27f);

    float result = noise1 * 0.45f + noise2 * 0.35f + noise3 * 0.20f;
    return result * 0.5f + 0.5f;
}

float3 CalculateCocktailColor(float2 waveXZ)
{
    float2 colorPosition = CalculateConvectionPosition(waveXZ);

    // 色境界を直線ではなく大きく蛇行させる
    float largeFlow = sin(colorPosition.y * 1.13f) * 0.28f;
    largeFlow += sin(colorPosition.y * 0.47f + colorPosition.x * 0.31f) * 0.18f;
    largeFlow += cos(colorPosition.y * 0.29f - colorPosition.x * 0.53f) * 0.10f;

    // A/B分離を判断する1次元値
    float colorAxis = (colorPosition.x + largeFlow) * gColorPatternScale;

    // 静止状態でも35%分の歪みは残す
    float distortionStrength = gColorDistortion * (0.35f + gMotionIntensity * 0.65f);
    colorAxis += CalculateColorNoise(colorPosition) * distortionStrength;

    // 0〜1のBalanceを-1〜+1へ変換して境界位置をずらす
    float balance = (gColorBalance - 0.5f) * 2.0f;
    colorAxis -= balance;

    // BlendWidthが0だとsmoothstepがゼロ除算になるのでガードする
    float blendWidth = max(gColorBlendWidth, 0.0001f);

    float separationMask = smoothstep(-blendWidth, blendWidth, colorAxis);
    float3 separatedColor = lerp(gColorA.rgb, gColorB.rgb, separationMask);

    // 完全混合色は平均ではなく加算。赤+青ならマゼンタになる
    float3 fullyMixedColor = saturate(gColorA.rgb + gColorB.rgb);

    float3 residualColor = lerp(gColorA.rgb, gColorB.rgb, CalculateResidualNoise(colorPosition));

    // Mix=0.5で最大になる山型の係数
    float middleStrength = gMixProgress * (1.0f - gMixProgress) * 4.0f;

    float3 baseMix = lerp(separatedColor, fullyMixedColor, gMixProgress);
    float3 finalColor = lerp(baseMix, residualColor, middleStrength * 0.22f);

    // 後半ほど強く均一化し、最終的に必ず完全混合色へ収束させる
    return lerp(finalColor, fullyMixedColor, gMixProgress * gMixProgress);
}

float4 main(WaterVaryings input) : SV_TARGET
{
    float3 normalWS = normalize(input.normalWS);
    float3 lightDirection = normalize(gMainLightDirection.xyz);
    float3 viewDirection = normalize(gCameraPositionWS - input.positionWS);

    float NdotL = saturate(dot(normalWS, lightDirection));
    float3 ambient = lerp(gAmbientGround.rgb, gAmbientSky.rgb, normalWS.y * 0.5f + 0.5f);

    float3 lighting = ambient + gMainLightColor.rgb * NdotL;
    float3 color = CalculateCocktailColor(input.wavePosition) * lighting;

    // Blinn-Phong スペキュラ
    float3 halfDirection = normalize(lightDirection + viewDirection);
    float specularPower = lerp(16.0f, 128.0f, gSmoothness);
    float specular = pow(saturate(dot(normalWS, halfDirection)), specularPower);

    color += specular * gMainLightColor.rgb * 0.45f;

    // 縁を不透明に寄せて液体らしくする。gFresnelStrength=0で元版と同じ
    float fresnel = pow(1.0f - saturate(dot(normalWS, viewDirection)), 5.0f);
    float alpha = saturate(gBaseColor.a + fresnel * gFresnelStrength);

    return float4(color, alpha);
}