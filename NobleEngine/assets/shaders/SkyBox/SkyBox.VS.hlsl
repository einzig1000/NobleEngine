// SkyBox.VS

struct VSInput
{
	float4 position : POSITION;
};

struct VSOutput
{
	float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

cbuffer WorldViewProjection : register(b0)
{
	float4x4 wvp;
}

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(input.position, wvp).xyww;
    output.texCoord = input.position.xyz;
	return output;
}