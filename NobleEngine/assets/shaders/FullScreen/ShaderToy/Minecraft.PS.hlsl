// Minecraft. Created by Reinder Nijhoff 2013
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// @reindernijhoff
//
// https://www.shadertoy.com/view/4ds3WS
//
// port of javascript minecraft: http://jsfiddle.net/uzMPU/
// original code by Markus Persson: https://twitter.com/notch/status/275331530040160256
// combined with voxel-shader by inigo quilez (https://www.shadertoy.com/view/4dfGzs)
//
// GLSL -> HLSL 変換。iChannel0/1(Shadertoy のノイズテクスチャ)はハッシュによる値ノイズに置き換え

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer Time : register(b0)
{
    float iTime;
};

cbuffer Resolution : register(b1)
{
    float2 iResolution; // 描画先RTのピクセルサイズ
};

#define SEALEVEL (-25.0f)
#define MAXSTEPS 180
//#define HOUSE

static const float3 sundir = normalize(float3(-0.5f, 0.6f, 0.7f));

// GLSL の mod は floor 基準(負でも 0〜y を返す)。HLSL の fmod は切り捨て基準で負の数で結果が変わるので自作する
float mod(float x, float y)
{
    return x - y * floor(x / y);
}
float2 mod(float2 x, float2 y)
{
    return x - y * floor(x / y);
}
float3 mod(float3 x, float3 y)
{
    return x - y * floor(x / y);
}

float hash(float n)
{
    return frac(sin(n) * 43758.5453f);
}
float hash(float3 x)
{
    float n = dot(x, float3(1.0f, 113.0f, 257.0f));
    return frac(sin(n) * 43758.5453f);
}
float3 hash3(float3 n)
{
    return frac(sin(n) * float3(653.5453123f, 4456.14123f, 165.340423f));
}

// 元は iChannel0(ノイズテクスチャ)の .yx を使った3D値ノイズ。格子の8隅をハッシュして補間する形に置き換え
float noise(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0f - 2.0f * f);

    float n000 = hash(p + float3(0.0f, 0.0f, 0.0f));
    float n100 = hash(p + float3(1.0f, 0.0f, 0.0f));
    float n010 = hash(p + float3(0.0f, 1.0f, 0.0f));
    float n110 = hash(p + float3(1.0f, 1.0f, 0.0f));
    float n001 = hash(p + float3(0.0f, 0.0f, 1.0f));
    float n101 = hash(p + float3(1.0f, 0.0f, 1.0f));
    float n011 = hash(p + float3(0.0f, 1.0f, 1.0f));
    float n111 = hash(p + float3(1.0f, 1.0f, 1.0f));

    return lerp(lerp(lerp(n000, n100, f.x), lerp(n010, n110, f.x), f.y),
                lerp(lerp(n001, n101, f.x), lerp(n011, n111, f.x), f.y), f.z);
}
// 元は iChannel0 の .x を引く2D値ノイズ。同じく格子の4隅をハッシュして補間する形に置き換え
float noise(float2 x)
{
    float2 p = floor(x);
    float2 f = frac(x);
    f = f * f * (3.0f - 2.0f * f);

    float n = p.x + p.y * 57.0f;
    float a = hash(n + 0.0f);
    float b = hash(n + 1.0f);
    float c = hash(n + 57.0f);
    float d = hash(n + 58.0f);
    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}
float sum(float3 v)
{
    return dot(v, float3(1.0f, 1.0f, 1.0f));
}

// port of minecraft

bool getMaterialColor(int i, float2 coord, out float3 color)
{
    // 16x16 tex
    float2 uv = floor(coord);

    float n = uv.x + uv.y * 347.0f + 4321.0f * float(i);
    float h = hash(n);

    float br = 1.0f - h * (96.0f / 255.0f);
    color = float3(150.0f / 255.0f, 108.0f / 255.0f, 74.0f / 255.0f); // 0x966C4A;

    float xm1 = mod((uv.x * uv.x * 3.0f + uv.x * 81.0f) / 4.0f, 4.0f);

    if (i == 1)
    {
        if (uv.y < (xm1 + 18.0f))
        {
            color = float3(106.0f / 255.0f, 170.0f / 255.0f, 64.0f / 255.0f); // 0x6AAA40;
        }
        else if (uv.y < (xm1 + 19.0f))
        {
            br = br * (2.0f / 3.0f);
        }
    }
    if (i == 4)
    {
        color = float3(127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f); // 0x7F7F7F;
    }
    if (i == 7)
    {
        color = float3(103.0f / 255.0f, 82.0f / 255.0f, 49.0f / 255.0f); // 0x675231;
        if (h < 0.5f)
        {
            br = br * (1.5f - mod(uv.x, 2.0f));
        }
    }
#ifdef HOUSE
    if (i == 5)
    {
        color = float3(181.0f / 255.0f, 58.0f / 255.0f, 21.0f / 255.0f); // 0xB53A15;
        if (mod(uv.x + (floor(uv.y / 4.0f) * 5.0f), 8.0f) == 0.0f || mod(uv.y, 4.0f) == 0.0f)
        {
            color = float3(188.0f / 255.0f, 175.0f / 255.0f, 165.0f / 255.0f); // 0xBCAFA5;
        }
    }
#endif
    if (i == 9)
    {
        color = float3(64.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f); // 0x4040ff;
    }
    if (i == 8)
    {
        color = float3(80.0f / 255.0f, 217.0f / 255.0f, 55.0f / 255.0f); // 0x50D937;
        if (h < 0.5f)
        {
            return false;
        }
    }
    if (i == 10)
    {
        color = float3(0.65f, 0.68f, 0.7f) * 1.35f;
        br = 1.0f;
    }
    color *= br;

    return true;
}

//=====================================================================
// Code by inigo quilez - iq/2013:
// (元にあった const mat3 m はどこからも使われていないので省略)

// 元は iChannel1(ノイズテクスチャ)を p*0.01 で引いていた。256x256 想定で 1テクセル = 256倍 の値ノイズに置き換え
float terrainTexture(float2 uv)
{
    return noise(uv * 256.0f);
}

float mapTerrain(float2 p)
{
    p *= 0.02f;

    float f;
    f = 0.500f * terrainTexture(p * 0.01f);
    f += 0.1250f * noise(p * 4.01f);
    return max(50.0f * f - 30.0f, SEALEVEL);
}

// static を付けないと定数バッファ扱い(読み取り専用)になるので注意
static float3 gro = float3(0.0f, 0.0f, 0.0f);

bool map(float3 c)
{
    float3 p = c + 0.5f;

    float f = mapTerrain(p.xz);

    float2 fc = floor(c.xz * 0.05f);
    float3 h = hash3(float3(fc * float2(213.123f, 2134.125f), mapTerrain(fc)));
    bool hit = false;

    if (h.z > 0.75f)
    {
        float2 tp = floor(fc * 20.0f + mod(h.yx * 154.43125f, 10.0f)) + 5.5f;
        float th = mapTerrain(tp); // 元は外側と同じ名前 h だったので改名
        if (th > SEALEVEL)
        {
            if (all(tp == p.xz))
                hit = c.y < th + 4.0f; // treetrunk
            if (distance(p, float3(tp.x, th + 6.0f, tp.y)) < 2.5f)
                hit = true; // leaves
        }
    }

    hit = c.y < f ? true : hit; // ground

    if (c.y > 8.0f &&
        sin((c.y - 8.0f) * (3.1415f / 32.0f)) * (10.0f / (c.y - 7.0f)) * noise(c * 0.08f + (0.7f * iTime) * float3(0.3f, 0.07f, 0.12f))
        > 0.6f)
        hit = true; // clouds

#ifdef HOUSE
    float2 hc = abs(c.xz - float2(32.0f, 130.0f)); // house
    if (all(hc < float2(6.0f, 10.0f)) && c.y < -hc.x - 12.0f)
    {
        hit = true;
        if (all(hc < float2(2.0f, 10.0f)) && c.y < -18.0f && c.y > -23.0f)
        {
            hit = false;
        }
        if (all(hc < float2(5.0f, 9.0f)) && c.y < -18.0f && c.y > -23.0f)
        {
            hit = false;
        }
    }
#endif

    if (distance(gro, c) < 1.5f)
        return false;

    return hit;
}


int mapMaterial(float3 c)
{
    int mat = 0;
    float3 p = c + 0.5f;

    float f = ceil(mapTerrain(p.xz));

    if (p.y <= f)
        mat = 1; // ground
    else if (p.y < f + 3.0f)
        mat = 7; // treetrunk
    else if (p.y < f + 10.0f)
        mat = 8; // leaves
    else
        mat = 10; // clouds

#ifdef HOUSE
    float2 hc = abs(c.xz - float2(32.0f, 130.0f));
    if (c.y < 0.0f && all(hc < float2(6.0f, 10.0f)))
    {
        mat = 5;
        if (!map(c + float3(0.0f, 1.0f, 0.0f))) mat = 6;
    }
#endif

    return mat;
}

float castRay(float3 ro, float3 rd, out float3 oVos, out float3 oDir)
{
    float3 pos = floor(ro);
    float3 ri = 1.0f / rd;
    float3 rs = (float3) sign(rd); // HLSL の sign は int を返すので float に戻す
    float3 dis = (pos - ro + 0.5f + rs * 0.5f) * ri;

    float3 mm = float3(0.0f, 0.0f, 0.0f);
    bool hit = false;

    [loop]
    for (int i = 0; i < MAXSTEPS; i++)
    {
        if (hit)
            break;
        mm = step(dis.xyz, dis.yxy) * step(dis.xyz, dis.zzx);
        dis += mm * rs * ri;
        pos += mm * rs;
        if (map(pos))
        {
            hit = true;
        }
    }

    float3 vos = pos;

    // intersect the cube
    float3 mini = (pos - ro + 0.5f - 0.5f * rs) * ri;
    float t = max(mini.x, max(mini.y, mini.z));

    oDir = mm;
    oVos = vos;

    return hit ? t : 0.0f;
}

float castVRay(float3 ro, float3 rd, float maxDist)
{
    float3 pos = floor(ro);
    float3 ri = 1.0f / rd;
    float3 rs = (float3) sign(rd);
    float3 dis = (pos - ro + 0.5f + rs * 0.5f) * ri;

    float res = 1.0f;

    [loop]
    for (int i = 0; i < 18; i++)
    {
        if (map(pos))
        {
            res = 0.0f;
            break;
        }
        float3 mm = step(dis.xyz, dis.yxy) * step(dis.xyz, dis.zzx);
        dis += mm * rs * ri;
        pos += mm * rs;
    }

    return res;
}

float3 path(float t)
{
    float2 p = 100.0f * sin(0.02f * t * float2(1.0f, 1.2f) + float2(0.1f, 0.9f));
    p += 50.0f * sin(0.04f * t * float2(1.3f, 1.0f) + float2(1.0f, 4.5f));

    return float3(p.x, mapTerrain(p) + 2.0f + 4.0f * (1.0f - cos(iTime * 0.1f)), p.y);
}


//=====================================================================
// Ambient occlusion

float4 edges(float3 vos, float3 nor, float3 dir)
{
    float3 v1 = vos + nor + dir.yzx;
    float3 v2 = vos + nor - dir.yzx;
    float3 v3 = vos + nor + dir.zxy;
    float3 v4 = vos + nor - dir.zxy;

    float4 res = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (map(v1))
        res.x = 1.0f;
    if (map(v2))
        res.y = 1.0f;
    if (map(v3))
        res.z = 1.0f;
    if (map(v4))
        res.w = 1.0f;

    return res;
}

float4 corners(float3 vos, float3 nor, float3 dir)
{
    float3 v1 = vos + nor + dir.yzx + dir.zxy;
    float3 v2 = vos + nor - dir.yzx + dir.zxy;
    float3 v3 = vos + nor - dir.yzx - dir.zxy;
    float3 v4 = vos + nor + dir.yzx - dir.zxy;

    float4 res = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (map(v1))
        res.x = 1.0f;
    if (map(v2))
        res.y = 1.0f;
    if (map(v3))
        res.z = 1.0f;
    if (map(v4))
        res.w = 1.0f;

    return res;
}

// sRGB → リニア(RTが _SRGB なので、書き込み時の自動変換と打ち消し合わせる)
float3 SRGBToLinear(float3 c)
{
    return lerp(c / 12.92f, pow((c + 0.055f) / 1.055f, 2.4f), step(0.04045f, c));
}

PSOutput main(PSInput input)
{
    PSOutput output;

    // Shadertoy の fragCoord は「左下原点・ピクセル単位」なので、左上原点の TexCoord を上下反転して作る
    float2 fragCoord = float2(input.TexCoord.x, 1.0f - input.TexCoord.y) * iResolution;

    // inputs
    float2 q = fragCoord.xy / iResolution.xy;
    float2 p = -1.0f + 2.0f * q;
    p.x *= iResolution.x / iResolution.y;

    float time = 2.0f * iTime; // 元は + 50.0 * mo.x(マウスで時間を進める)。不要なので省略

    // camera
    float cr = 0.2f * cos(0.1f * iTime);
    float3 ro = path(time);
    float3 ta = path(time + 4.0f);
    ta.y = ro.y;
    gro = ro;

    // build ray
    float3 ww = normalize(ta - ro);
    float3 uu = normalize(cross(float3(sin(cr), cos(cr), 0.0f), ww));
    float3 vv = normalize(cross(ww, uu));
    float r2 = p.x * p.x * 0.32f + p.y * p.y;
    p *= (7.0f - sqrt(37.5f - 11.5f * r2)) / (r2 + 1.0f);
    float3 rd = normalize(p.x * uu + p.y * vv + 2.5f * ww);

    float sun = clamp(dot(sundir, rd), 0.0f, 1.0f);
    float3 col = float3(0.6f, 0.71f, 0.75f) - rd.y * 0.2f * float3(1.0f, 0.5f, 1.0f) + 0.15f * 0.5f;
    col += 0.2f * float3(1.0f, 0.6f, 0.1f) * pow(sun, 8.0f);
    col *= 0.95f;

    float3 vos, dir;
    float t = castRay(ro, rd, vos, dir);

    if (t > 0.0f)
    {
        float3 nor = -dir * (float3) sign(rd);

        float3 pos = ro + rd * t;
        int mMat = mapMaterial(vos);
        float3 mpos = mod(pos * 16.0f, 16.0f);

        if (mMat == 1)
        {
            if (map(vos + float3(0.0f, 1.0f, 0.0f)))
            {
                mMat = hash(vos) > 0.5f ? 2 : 4;
                if (map(vos + float3(0.0f, 2.0f, 0.0f)))
                    mMat = 4;
            }
            if (vos.y < SEALEVEL)
                mMat = 9;
        }

        float3 mCol;
        float2 texCoord = nor.y != 0.0f ? mpos.xz
                        : nor.x != 0.0f ? -mpos.zy + float2(32.0f, 32.0f)
                        : -mpos.xy + float2(32.0f, 32.0f);
        getMaterialColor(mMat, texCoord, mCol);

        // lighting
        float dif = clamp(dot(nor, sundir), 0.0f, 1.0f);
        float sha = 0.0f;
        if (dif > 0.01f)
            sha = castVRay(pos + nor * 0.01f, sundir, 32.0f);
        float bac = clamp(dot(nor, normalize(sundir * float3(-1.0f, 0.0f, -1.0f))), 0.0f, 1.0f);
        float sky = 0.5f + 0.5f * nor.y;

        // ambient occlusion
        float4 ed = edges(vos, nor, dir);
        float4 co = corners(vos, nor, dir);
        float3 uvw = pos - vos;
        float2 uv = float2(dot(dir.yzx, uvw), dot(dir.zxy, uvw));

        float occ = 0.0f;
        // (for edges)
        occ += (uv.x) * ed.x;
        occ += (1.0f - uv.x) * ed.y;
        occ += (uv.y) * ed.z;
        occ += (1.0f - uv.y) * ed.w;
        // (for corners)
        occ += (uv.y * uv.x) * co.x * (1.0f - ed.x) * (1.0f - ed.z);
        occ += (uv.y * (1.0f - uv.x)) * co.y * (1.0f - ed.z) * (1.0f - ed.y);
        occ += ((1.0f - uv.y) * (1.0f - uv.x)) * co.z * (1.0f - ed.y) * (1.0f - ed.w);
        occ += ((1.0f - uv.y) * uv.x) * co.w * (1.0f - ed.w) * (1.0f - ed.x);
        occ = 1.0f - occ / 8.0f;
        occ = occ * occ;
        occ = occ * occ;

        float3 lin = float3(0.0f, 0.0f, 0.0f);
        lin += 4.0f * dif * float3(1.0f, 1.0f, 1.0f) * (0.5f + 0.5f * occ) * (0.25f + 0.75f * sha);
        lin += 1.8f * bac * float3(1.0f, 0.5f, 1.0f) * (0.5f + 0.5f * occ);
        lin += 4.0f * sky * float3(0.6f, 0.71f, 0.75f) * occ;

        if (mMat == 10)
        {
            col = lerp(col, mCol * lin * 0.6f, 0.3f);
        }
        else
        {
            // atmospheric
            col = lerp(mCol * lin * 0.2f, col, 1.0f - exp(-0.0000001f * t * t * t));
        }
    }

    col += 0.2f * float3(1.0f, 0.4f, 0.2f) * pow(sun, 3.0f);

    // gamma(負の値で pow が NaN にならないよう max を挟む)
    col = pow(max(col, 0.0f), 0.45f);

    // contrast
    col = col * 0.25f + 0.75f * col * col * (3.0f - 2.0f * col);

    col = clamp(col, 0.0f, 1.0f);

    // vignetting
    col *= 0.5f + 0.5f * pow(16.0f * q.x * q.y * (1.0f - q.x) * (1.0f - q.y), 0.1f);

    output.Color = float4(SRGBToLinear(col), 1.0f);
    return output;
}
