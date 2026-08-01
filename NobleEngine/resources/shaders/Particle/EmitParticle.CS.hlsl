
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
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

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
            
            int32_t freeListIndex;
            //FreeListのIndexを1つ前に設定し、現在のIndexを取得する
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < kMaxParticles)
            {
                uint32_t particleIndex = gFreeList[freeListIndex];
                // カウント分Particleを射出する
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].scale *= 0.1f;
                //gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].translate = gEmitter.translate + (generator.Generate3d() * 2.0f - 1.0f) * gEmitter.radius;
                float3 randomDirection = generator.Generate3d();
                randomDirection = randomDirection * 2.0f - 1.0f;
                gParticles[particleIndex].velocity = randomDirection * 0.01f;
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].lifeTime = 1.0f;
                gParticles[particleIndex].currentTime = 0.0f;
            }
            else
            {
                //発生させられなかったので、減らしてしまった分もとに戻す。これを忘れると発生させられなかった分だけどんどんIndexが減ってしまう
                InterlockedAdd(gFreeListIndex[0], 1);
                //Emit中にParticleは消えないので、この後発生することはないためbreakして終わらせる
                break;
            }
        }
    }
}
