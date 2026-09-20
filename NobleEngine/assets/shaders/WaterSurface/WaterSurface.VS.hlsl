// WaterSurface.VS.hlsl
// 液面の波による頂点変形。天面を事前分割したメッシュを前提とする。
#include "WaterSurface.hlsli"

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer TransformBuffer : register(b0)
{
    row_major float4x4 gWorld;
    row_major float4x4 gWorldInverseTranspose; // 非一様スケール対応の法線行列
    row_major float4x4 gViewProjection;
}

cbuffer WaveBuffer : register(b1)
{
    float4 gRelativeScale; // 波の密度をオブジェクトScaleから独立させる補正
    float4 gWaveAxisXWS; // 波専用座標系の3軸。w未使用
    float4 gWaveAxisYWS;
    float4 gWaveAxisZWS;

    float gSurfaceY; // 液面の World Y
    float gCommonWorldScale; // モデルサイズ基準。波高に掛ける
    float gSideWaveDepth; // 液面から下へ何mまで側面を追従させるか
    float gWaveHeight;

    float gWaveFrequency;
    float gWaveSpeed;
    float gMotionHeightBoost; // MotionIntensity による波高増加量
    float gMotionIntensity; // グラスの動きの激しさ 0〜1

    float gMotionWaveTime; // CPU で加算し続けるアニメーション時間
    float3 gWavePadding;
}

// 波と色で共用する2次元座標を作る
float2 GetWavePosition(float3 positionOS)
{
    float3 relativePosition = float3(
        positionOS.x * gRelativeScale.x,
        positionOS.y * gRelativeScale.y,
        positionOS.z * gRelativeScale.z);

    // Transform階層の回転とは独立に波の向きを決めるため、CPUから渡された3軸で再構成する
    float3 rotatedPosition =
        gWaveAxisXWS.xyz * relativePosition.x +
        gWaveAxisYWS.xyz * relativePosition.y +
        gWaveAxisZWS.xyz * relativePosition.z;

    return rotatedPosition.xz;
}

// 液面から下へ離れるほど波の適用率を下げる
float CalculateWaveInfluence(float worldY)
{
    float depth = max(gSideWaveDepth * gCommonWorldScale, 0.0001f);

    float distanceBelowSurface = gSurfaceY - worldY;
    if (distanceBelowSurface <= 0.0f)
    {
        return 1.0f;
    }

    return 1.0f - smoothstep(0.0f, depth, distanceBelowSurface);
}

// 波高と、その解析微分から求めた法線を同時に返す
float CalculateWaveAndNormal(float2 waveXZ, out float3 waveNormalWS)
{
    float time = gMotionWaveTime;
    float motionHeightMultiplier = 1.0f + gMotionIntensity * gMotionHeightBoost;

    // Z側は周波数×0.83・速度×0.71・逆方向にして格子模様になるのを避ける
    float phaseX = waveXZ.x * gWaveFrequency + time * gWaveSpeed;
    float phaseZ = waveXZ.y * gWaveFrequency * 0.83f - time * gWaveSpeed * 0.71f;

    float sinX = sin(phaseX);
    float cosX = cos(phaseX);
    float sinZ = sin(phaseZ);
    float cosZ = cos(phaseZ);

    // main() で実際に加算される変位の振幅
    float amplitude = gWaveHeight * motionHeightMultiplier * gCommonWorldScale;

    // h = sin(phaseX) * cos(phaseZ) * amplitude の偏微分
    float dhdx = cosX * cosZ * gWaveFrequency * amplitude;
    float dhdz = -sinX * sinZ * gWaveFrequency * 0.83f * amplitude;

    waveNormalWS = normalize(float3(-dhdx, 1.0f, -dhdz));

    return sinX * cosZ * gWaveHeight * motionHeightMultiplier;
}

WaterVaryings main(VSInput input)
{
    float3 positionOS = input.position.xyz;

    float3 positionWS = mul(float4(positionOS, 1.0f), gWorld).xyz;
    float3 normalWS = normalize(mul(float4(input.normal, 0.0f), gWorldInverseTranspose).xyz);

    float2 wavePosition = GetWavePosition(positionOS);
    float influence = CalculateWaveInfluence(positionWS.y);

    if (influence > 0.0f)
    {
        float3 waveNormalWS;
        float wave = CalculateWaveAndNormal(wavePosition, waveNormalWS);

        // 重力方向に持ち上げるので World Y へ直接加算する
        positionWS.y += wave * gCommonWorldScale * influence;

        // 上を向いている面ほど波の法線を強く反映させる。
        // 側面(法線が水平)は元の法線を保つ。
        float upFacing = saturate(dot(normalWS, float3(0.0f, 1.0f, 0.0f)));
        normalWS = normalize(lerp(normalWS, waveNormalWS, influence * upFacing));
    }

    WaterVaryings output;
    output.positionWS = positionWS;
    output.normalWS = normalWS;
    output.wavePosition = wavePosition;
    output.positionCS = mul(float4(positionWS, 1.0f), gViewProjection);
    return output;
}