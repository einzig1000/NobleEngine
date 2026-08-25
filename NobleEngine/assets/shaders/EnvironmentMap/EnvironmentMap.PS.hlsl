// PunctualLight.PS
static const int LIGHT_TYPE_DIRECTIONAL = 0;
static const int LIGHT_TYPE_POINT = 1;
static const int LIGHT_TYPE_SPOT = 2;

struct PunctualLight
{
    // 共通
    float3 color; // 光の色
    float intensity; // 明るさスケール

    // Directional / Spot 用
    float3 direction; // 光の向き（正規化前提）
    float spotCos; // スポットライトの内側コーンの cos(角度)

    // Point / Spot 用
    float3 position; // 光源位置（ワールド）
    float range; // 有効距離

    int type; // 0:Dir, 1:Point, 2:Spot
    float3 _pad;
};

struct Material
{
    float3 diffuseColor;
    float shininess; // 鏡面ハイライトの鋭さ
    float3 specularColor;
    float _pad0;
};

struct LightResult
{
    float3 diffuse;
    float3 specular;
};


cbuffer CameraCB : register(b0)
{
    float3 gCameraWorldPosition;
    float _pad1;
};

cbuffer LightCB : register(b1)
{
    PunctualLight gLights[4];
    int gLightCount;
    float3 gAmbientColor;
};

cbuffer MaterialCB : register(b2)
{
    Material gMaterial;
};

cbuffer TextureIndex : register(b3)
{
    int textureIndex;
};

cbuffer DdsTextureIndex : register(b4)
{
    int ddsTextureIndex;
}


Texture2D<float4> textures[] : register(t0, space0);
TextureCube<float4> ddsTextures[] : register(t1, space1);
SamplerState gSampler : register(s0);


struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};




// 距離減衰（0〜range で 1→0 に落ちる）
float CalcDistanceAttenuation(float dist, float range)
{
    float a = saturate(1.0f - dist / range);
    // 少しカーブさせて自然に
    return a * a;
}

// Lambert 拡散
float3 CalcDiffuse(float3 N, float3 L, float3 lightColor, float intensity)
{
    float NdotL = saturate(dot(N, L));
    return lightColor * intensity * NdotL;
}

// Blinn-Phong 鏡面
float3 CalcSpecular(float3 N, float3 L, float3 V, float3 specColor, float shininess, float intensity)
{
    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    float spec = pow(NdotH, shininess);
    return specColor * spec * intensity;
}

// 1 つの PunctualLight を評価
LightResult EvaluatePunctualLight(PunctualLight light, float3 worldPos, float3 N, float3 V)
{
    LightResult result = (LightResult) 0;

    if (light.type == LIGHT_TYPE_DIRECTIONAL)
    {
        float3 L = normalize(-light.direction);

        float3 diffuse = CalcDiffuse(N, L, light.color, light.intensity);
        float3 specular = CalcSpecular(N, L, V, gMaterial.specularColor, gMaterial.shininess, light.intensity);
        
        result.diffuse = diffuse;
        result.specular = specular;
    }
    else if (light.type == LIGHT_TYPE_POINT)
    {
        // Point: 位置からのベクトル
        float3 Lvec = light.position - worldPos;
        float dist = length(Lvec);
        float3 L = Lvec / max(dist, 1e-4);
        
        float att = CalcDistanceAttenuation(dist, light.range);
        
        float3 diffuse = CalcDiffuse(N, L, light.color, light.intensity);
        float3 specular = CalcSpecular(N, L, V, gMaterial.specularColor, gMaterial.shininess, light.intensity);
        
        result.diffuse = diffuse * att;
        result.specular = specular * att;
    }
    else if (light.type == LIGHT_TYPE_SPOT)
    {
        // Spot: Point + 角度減衰
        float3 Lvec = light.position - worldPos;
        float dist = length(Lvec);
        float3 L = Lvec / max(dist, 1e-4);

        // 距離減衰
        float attDist = CalcDistanceAttenuation(dist, light.range);

        // 角度減衰（direction はライトが照らす方向）
        float3 spotDir = normalize(-light.direction); // direction を「光が飛ぶ方向の逆」として扱うならここは -direction で調整
        float cosTheta = dot(L, spotDir);

        // 内側コーンを spotCos として、それより外は減衰
        float attAngle = saturate((cosTheta - light.spotCos) / (1.0f - light.spotCos));
        attAngle *= attAngle; // 少しカーブ

        float att = attDist * attAngle;

        float3 diffuse = CalcDiffuse(N, L, light.color, light.intensity);
        float3 specular = CalcSpecular(N, L, V, gMaterial.specularColor, gMaterial.shininess, light.intensity);
        
        result.diffuse = diffuse * att;
        result.specular = specular * att;
    }

    return result;
}

PSOutput main(PSInput input)
{
    PSOutput output;

    float3 Normalize = normalize(input.normal);
    float3 posToCamera = normalize(gCameraWorldPosition - input.worldPosition);
    float3 reflectVector = reflect(-posToCamera, Normalize);

    float4 texColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    float4 ddsColor = ddsTextures[ddsTextureIndex].Sample(gSampler, reflectVector);
    
    float3 diffuseLighting = gAmbientColor; // 環境光はDiffuseに乗る
    float3 specularLighting = 0.0f;
    
    // すべての PunctualLight を合成
    [loop]
    for (int i = 0; i < gLightCount; ++i)
    {
        LightResult lr = EvaluatePunctualLight(gLights[i], input.worldPosition, Normalize, posToCamera);
        diffuseLighting += lr.diffuse;
        specularLighting += lr.specular;
    }

    //float3 finalColor =
    //(texColor.rgb * gMaterial.diffuseColor * diffuseLighting)
    //+ specularLighting
    //+ ddsColor.rgb * 0.5f;
    float3 finalColor =
    (gMaterial.diffuseColor * diffuseLighting)
    + specularLighting
    + ddsColor.rgb * 0.5f;
    
    output.color = float4(finalColor, texColor.a);
    return output;
}
