
struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};

static const uint kMaxParticles = 1024;

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

cbuffer gPerFrame : register(b0)
{
    float deltaTime;
};


[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        // alphaが0のparticleは死んでいるとみなして更新しない
        if (gParticles[particleIndex].color.a != 0)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
        else if (gParticles[particleIndex].lifeTime != 0.0f)
        {
            gParticles[particleIndex].scale = float32_t3(0.0f, 0.0f, 0.0f);
            gParticles[particleIndex].lifeTime = 0.0f;
            // 死んでいるparticleはfreeListに戻す
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            //最新のFreeListIndexの場所に死んだParticleのIndexを設定する。
            if ((freeListIndex + 1) < kMaxParticles)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                //ここにくるはずはない。きたら何かが間違っているが、安全策をうっておく
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}
