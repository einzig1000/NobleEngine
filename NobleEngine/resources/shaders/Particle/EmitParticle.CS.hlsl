
static const uint kMaxParticles = 1024;

struct EmitterSphere
{
    float3 translate; // 座標
    float radius; // 射出半径
    uint count; // 射出数
    float frequency; // 射出頻度
    float frequencyTime; // 射出頻度タイマ
    uint emit; // 射出するかどうか
};

struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};

struct Seed
{
    float3 seed;
};

float rand3dTo1d(float3 value)
{
    float3 smallValue = sin(value);
    float random = dot(smallValue, float3(12.9898, 78.233, 37.719));
    random = frac(sin(random) * 143758.5453);
    return random;
}

float3 rand3dTo3d(float3 value)
{
    float r0 = rand3dTo1d(value);
    float r1 = rand3dTo1d(value + float3(12.34, 56.78, 90.12));
    float r2 = rand3dTo1d(value + float3(98.76, 54.32, 10.12));
    return float3(r0, r1, r2);
}

class RandomGenerator
{
    float3 seed;
    float3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
};

ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<Seed> gSeed : register(b1);
RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<uint> gFreeCounter : register(u1);

// 今回スレッド数は1。複数のEmitterを扱い、同時に処理したいような場合は適宜スレッド数を増やすと良い
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (gEmitter.emit != 0) // 射出許可が出たので射出
    {
        RandomGenerator generator;
        generator.seed = gSeed.seed;

        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int particleindex;
            InterlockedAdd(gFreeCounter[0], 1, particleindex);
            
            if (particleindex < kMaxParticles)
            {
                // カウント分Particleを射出する
                gParticles[particleindex].scale = generator.Generate3d();
                gParticles[particleindex].translate = generator.Generate3d();
                gParticles[particleindex].color.rgb = generator.Generate3d();
                gParticles[particleindex].color.a = 1.0f;
            }
        }
    }
}
