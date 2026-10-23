// "Seascape" by Alexander Alekseev aka TDM - 2014 (Shadertoy)
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// GLSL -> HLSL 変換

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

// 名前は Shadertoy のまま(time にすると main の「float time = time * 0.3」が自分自身を読む)
cbuffer Time : register(b0)
{
    float iTime;
};

cbuffer Resolution : register(b1)
{
    float2 iResolution; // 描画先RTのピクセルサイズ
};

static const int NUM_STEPS = 32;
static const float PI = 3.141592f;
static const float EPSILON = 1e-3f;
#define EPSILON_NRM (0.1f / iResolution.x)
//#define AA

// sea
static const int ITER_GEOMETRY = 3;
static const int ITER_FRAGMENT = 5;
static const float SEA_HEIGHT = 0.6f;
static const float SEA_CHOPPY = 4.0f;
static const float SEA_SPEED = 0.8f;
static const float SEA_FREQ = 0.16f;
static const float3 SEA_BASE = float3(0.0f, 0.09f, 0.18f);
static const float3 SEA_WATER_COLOR = float3(0.8f, 0.9f, 0.6f) * 0.6f;
#define SEA_TIME (1.0f + iTime * SEA_SPEED)
// GLSLと同じ並びで書き、掛け算の順番を逆にする(uv * m → mul(m, uv))
static const float2x2 octave_m = float2x2(1.6f, 1.2f, -1.2f, 1.6f);

// math
// GLSLの m[i] は「列」、HLSLの m[i] は「行」なので、これはGLSL版の転置になる
// → 使う側で dir * m を mul(m, dir) にして帳尻を合わせる
float3x3 fromEuler(float3 ang)
{
    float2 a1 = float2(sin(ang.x), cos(ang.x));
    float2 a2 = float2(sin(ang.y), cos(ang.y));
    float2 a3 = float2(sin(ang.z), cos(ang.z));
    float3x3 m;
    m[0] = float3(a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x);
    m[1] = float3(-a2.y * a1.x, a1.y * a2.y, a2.x);
    m[2] = float3(a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x, a2.y * a3.y);
    return m;
}
float hash(float2 p)
{
    float h = dot(p, float2(127.1f, 311.7f));
    return frac(sin(h) * 43758.5453123f);
}
float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0f - 2.0f * f);
    return -1.0f + 2.0f * lerp(lerp(hash(i + float2(0.0f, 0.0f)),
                                    hash(i + float2(1.0f, 0.0f)), u.x),
                               lerp(hash(i + float2(0.0f, 1.0f)),
                                    hash(i + float2(1.0f, 1.0f)), u.x), u.y);
}

// lighting
float diffuse(float3 n, float3 l, float p)
{
    return pow(dot(n, l) * 0.4f + 0.6f, p);
}
float specular(float3 n, float3 l, float3 e, float s)
{
    float nrm = (s + 8.0f) / (PI * 8.0f);
    return pow(max(dot(reflect(e, n), l), 0.0f), s) * nrm;
}

// sky
float3 getSkyColor(float3 e)
{
    e.y = (max(e.y, 0.0f) * 0.8f + 0.2f) * 0.8f;
    return float3(pow(1.0f - e.y, 2.0f), 1.0f - e.y, 0.6f + (1.0f - e.y) * 0.4f) * 1.1f;
}

// sea
float sea_octave(float2 uv, float choppy)
{
    uv += noise(uv);
    float2 wv = 1.0f - abs(sin(uv));
    float2 swv = abs(cos(uv));
    wv = lerp(wv, swv, wv);
    return pow(1.0f - pow(wv.x * wv.y, 0.65f), choppy);
}

float map(float3 p)
{
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz;
    uv.x *= 0.75f;

    float d, h = 0.0f;
    for (int i = 0; i < ITER_GEOMETRY; i++)
    {
        d = sea_octave((uv + SEA_TIME) * freq, choppy);
        d += sea_octave((uv - SEA_TIME) * freq, choppy);
        h += d * amp;
        uv = mul(octave_m, uv);
        freq *= 1.9f;
        amp *= 0.22f;
        choppy = lerp(choppy, 1.0f, 0.2f);
    }
    return p.y - h;
}

float map_detailed(float3 p)
{
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz;
    uv.x *= 0.75f;

    float d, h = 0.0f;
    for (int i = 0; i < ITER_FRAGMENT; i++)
    {
        d = sea_octave((uv + SEA_TIME) * freq, choppy);
        d += sea_octave((uv - SEA_TIME) * freq, choppy);
        h += d * amp;
        uv = mul(octave_m, uv);
        freq *= 1.9f;
        amp *= 0.22f;
        choppy = lerp(choppy, 1.0f, 0.2f);
    }
    return p.y - h;
}

float3 getSeaColor(float3 p, float3 n, float3 l, float3 eye, float3 dist)
{
    float fresnel = clamp(1.0f - dot(n, -eye), 0.0f, 1.0f);
    fresnel = min(fresnel * fresnel * fresnel, 0.5f);

    float3 reflected = getSkyColor(reflect(eye, n));
    float3 refracted = SEA_BASE + diffuse(n, l, 80.0f) * SEA_WATER_COLOR * 0.12f;

    float3 color = lerp(refracted, reflected, fresnel);

    float atten = max(1.0f - dot(dist, dist) * 0.001f, 0.0f);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18f * atten;

    color += specular(n, l, eye, 600.0f * rsqrt(dot(dist, dist)));

    return color;
}

// tracing
float3 getNormal(float3 p, float eps)
{
    float3 n;
    n.y = map_detailed(p);
    n.x = map_detailed(float3(p.x + eps, p.y, p.z)) - n.y;
    n.z = map_detailed(float3(p.x, p.y, p.z + eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float heightMapTracing(float3 ori, float3 dir, out float3 p)
{
    float tm = 0.0f;
    float tx = 1000.0f;
    float hx = map(ori + dir * tx);
    if (hx > 0.0f)
    {
        p = ori + dir * tx;
        return tx;
    }
    float hm = map(ori);
    for (int i = 0; i < NUM_STEPS; i++)
    {
        float tmid = lerp(tm, tx, hm / (hm - hx));
        p = ori + dir * tmid;
        float hmid = map(p);
        if (hmid < 0.0f)
        {
            tx = tmid;
            hx = hmid;
        }
        else
        {
            tm = tmid;
            hm = hmid;
        }
        if (abs(hmid) < EPSILON)
        {
            break;
        }
    }
    return lerp(tm, tx, hm / (hm - hx));
}

float3 getPixel(float2 coord, float time)
{
    float2 uv = coord / iResolution.xy;
    uv = uv * 2.0f - 1.0f;
    uv.x *= iResolution.x / iResolution.y;

    // ray
    float3 ang = float3(sin(time * 3.0f) * 0.1f, sin(time) * 0.2f + 0.3f, time);
    float3 ori = float3(0.0f, 3.5f, time * 5.0f);
    float3 dir = normalize(float3(uv.xy, -2.0f));
    dir.z += length(uv) * 0.14f;
    dir = mul(fromEuler(ang), normalize(dir)); // GLSL: normalize(dir) * fromEuler(ang)

    // tracing
    float3 p;
    heightMapTracing(ori, dir, p);
    float3 dist = p - ori;
    float3 n = getNormal(p, dot(dist, dist) * EPSILON_NRM);
    float3 light = normalize(float3(0.0f, 1.0f, 0.8f));

    // color
    return lerp(
        getSkyColor(dir),
        getSeaColor(p, n, light, dir, dist),
        pow(smoothstep(0.0f, -0.02f, dir.y), 0.2f));
}

// sRGB → リニア(RTが _SRGB なので、書き込み時の自動変換と打ち消し合わせる)
float3 SRGBToLinear(float3 c)
{
    // ベクトルの三項演算子は HLSL 2021 でエラーになるので lerp + step で書く
    return lerp(c / 12.92f, pow((c + 0.055f) / 1.055f, 2.4f), step(0.04045f, c));
}

// main
PSOutput main(PSInput input)
{
    PSOutput output;

    // Shadertoy の fragCoord は「左下原点・ピクセル単位」なので、左上原点の TexCoord を上下反転して作る
    float2 fragCoord = float2(input.TexCoord.x, 1.0f - input.TexCoord.y) * iResolution;

    float time = iTime * 0.3f; // 元は + iMouse.x * 0.01(マウスで時間を進める)。不要なので省略

#ifdef AA
    float3 color = float3(0.0f, 0.0f, 0.0f);
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            float2 uv = fragCoord + float2(i, j) / 3.0f;
            color += getPixel(uv, time);
        }
    }
    color /= 9.0f;
#else
    float3 color = getPixel(fragCoord, time);
#endif

    // post
    // 負の値で pow が NaN にならないよう max を挟む
    float3 shadertoyColor = pow(max(color, 0.0f), 0.65f);
    output.Color = float4(SRGBToLinear(shadertoyColor), 1.0f);
    return output;
}
