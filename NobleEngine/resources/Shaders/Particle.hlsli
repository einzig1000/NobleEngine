// Particle.hlsli


struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};
struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    float32_t shininess;
    float32_t3 _padding;
};
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct WorldMatrix
{
    float32_t4x4 World;
};

struct ViewProjectionMatrix
{
    float32_t4x4 ViewProjection;
};
struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    int mode;
};

struct ParticleInf
{
    uint scaleRandom;
    float32_t3 scale;
    float32_t3 scaleMIN;
    float32_t3 scaleMAX;
    uint scaleVelocityRandom;
    float32_t3 scaleVelocity;
    float32_t3 scaleVelocityMIN;
    float32_t3 scaleVelocityMAX;
    uint scaleAccelerationRandom;
    float32_t3 scaleAcceleration;
    float32_t3 scaleAccelerationMIN;
    float32_t3 scaleAccelerationMAX;
    
    uint rotateRandom;
    float32_t3 rotate;
    float32_t3 rotateMIN;
    float32_t3 rotateMAX;
    uint rotateVelocityRandom;
    float32_t3 rotateVelocity;
    float32_t3 rotateVelocityMIN;
    float32_t3 rotateVelocityMAX;
    uint rotateAccelerationRandom;
    float32_t3 rotateAcceleration;
    float32_t3 rotateAccelerationMIN;
    float32_t3 rotateAccelerationMAX;
    
    uint translateRandom;
    float32_t3 translate;
    float32_t3 translateMIN;
    float32_t3 translateMAX;
    uint translateVelocityRandom;
    float32_t3 translateVelocity;
    float32_t3 translateVelocityMIN;
    float32_t3 translateVelocityMAX;
    uint translateAccelerationRandom;
    float32_t3 translateAcceleration;
    float32_t3 translateAccelerationMIN;
    float32_t3 translateAccelerationMAX;
    
    float32_t4x4 World;
    float32_t4x4 WVP;
    
    uint32_t liveTime;
    float32_t4 color;
    uint isBillboard;
};