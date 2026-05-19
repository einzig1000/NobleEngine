#include "Utilities/PerlinNoise.h"

PerlinNoise::PerlinNoise(uint32_t seed)
{
    pen.resize(256);
    std::iota(pen.begin(), pen.end(), 0);
    std::mt19937 gen(seed);
    std::shuffle(pen.begin(), pen.end(), gen);
    // 512長に拡張
    pen.insert(pen.end(), pen.begin(), pen.end());
}

float PerlinNoise::noise(float x, float y) const
{
    // 単純な実装: 周期なし
    int xi = static_cast<int>(std::floor(x)) & 255;
    int yi = static_cast<int>(std::floor(y)) & 255;
    float xf = x - std::floor(x);
    float yf = y - std::floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = pen[pen[xi] + yi];
    int ab = pen[pen[xi] + yi + 1];
    int ba = pen[pen[xi + 1] + yi];
    int bb = pen[pen[xi + 1] + yi + 1];

    float x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    float x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
    float res = lerp(x1, x2, v);

    // Perlin の出力は概ね -1..1 なので 0..1 に正規化
    return (res + 1.0f) / 2.0f;
}

float PerlinNoise::fade(float t)
{
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y)
{
    // hash の下位ビットで勾配ベクトルを選ぶ（簡易）
    switch (hash & 3)
    {
    case 0: return  x + y;
    case 1: return -x + y;
    case 2: return  x - y;
    case 3: return -x - y;
    default: return 0;
    }
}

float fractalPerlin(const PerlinNoise& pn, float x, float y, int octaves, float persistence)
{

    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    for (int i = 0; i < octaves; ++i)
    {
        total += pn.noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    return total / maxValue; // 0..1 に近い値
}