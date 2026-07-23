

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
RWStructuredBuffer<uint> gFreeCounter : register(u1);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        gParticles[particleIndex] = (Particle) 0;
        //gParticles[particleIndex].translate = float3(0.0f, 0.0f, 0.0f);
        //gParticles[particleIndex].scale = float3(1.0f, 1.0f, 1.0f);
        //gParticles[particleIndex].lifeTime = 0.0f;
        //gParticles[particleIndex].velocity = float3(0.0f, 0.0f, 0.0f);
        //gParticles[particleIndex].currentTime = 0.0f;
        //gParticles[particleIndex].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
