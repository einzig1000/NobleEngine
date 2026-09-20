
#ifndef WATER_SURFACE_HLSLI
#define WATER_SURFACE_HLSLI

struct WaterVaryings
{
    float4 positionCS : SV_POSITION;
    float3 positionWS : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float2 wavePosition : TEXCOORD2;
};

#endif