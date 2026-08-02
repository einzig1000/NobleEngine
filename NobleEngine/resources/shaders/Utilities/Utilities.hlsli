

float4 UnpackColor(uint c)
{
    float r = ((c >> 24) & 0xFF) / 255.0f;
    float g = ((c >> 16) & 0xFF) / 255.0f;
    float b = ((c >> 8) & 0xFF) / 255.0f;
    float a = (c & 0xFF) / 255.0f;
    return float4(r, g, b, a);
}

float rand1dTo1d(float value)
{
    float smallValue = sin(value);
    float random = smallValue * 12.9898;
    random = frac(sin(random) * 143758.5453);
    return random;
}

float rand1dto1d(uint value)
{
    float smallValue = sin(value);
    float random = smallValue * 12.9898;
    random = frac(sin(random) * 143758.5453);
    return random;
}

float rand2dTo1d(float2 value)
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, float2(12.9898, 78.233));
    random = frac(sin(random) * 143758.5453);
    return random;
}

float rand3dTo1d(float3 value)
{
    float3 smallValue = sin(value);
    float random = dot(smallValue, float3(12.9898, 78.233, 37.719));
    random = frac(sin(random) * 143758.5453);
    return random;
}

float rand4dTo1d(float4 value)
{
    float4 smallValue = sin(value);
    float random = dot(smallValue, float4(12.9898, 78.233, 37.719, 24.123));
    random = frac(sin(random) * 143758.5453);
    return random;
}

float4 rand3dTo4d(float3 value)
{
    float4 smallValue = sin(float4(value, 0.0f));
    float4 random = smallValue * float4(12.9898, 78.233, 37.719, 24.123);
    random = frac(sin(random) * float4(143758.5453, 143758.5453, 143758.5453, 143758.5453));
    return random;
}

