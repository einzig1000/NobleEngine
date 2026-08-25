#include "../3.0.0/Block.hlsli"

cbuffer SrvIndex : register(b0)
{
    // グループごとの露出面数のSRVスロット
    uint bakedFaceCountPerGroupSrvIndex;
};

cbuffer ChunkInfo : register(b1)
{
    float3 chunkWorldOrigin;
    float blockSize;
    uint3 chunkDim;
    uint _pad0;
};

cbuffer WVP : register(b2)
{
    float4x4 wvp;
}

struct Payload
{
    uint groupIndices[32];
};

groupshared Payload gPayload;
groupshared uint gVisibleCount;

// AABBの8頂点をwvpでクリップ空間に変換し、6平面すべてに対して「8頂点全部が外側」なら不可視と判定
bool AABBInFrustum(float3 localMin, float3 localMax)
{
    float3 corners[8] =
    {
        float3(localMin.x, localMin.y, localMin.z),
        float3(localMax.x, localMin.y, localMin.z),
        float3(localMin.x, localMax.y, localMin.z),
        float3(localMax.x, localMax.y, localMin.z),
        float3(localMin.x, localMin.y, localMax.z),
        float3(localMax.x, localMin.y, localMax.z),
        float3(localMin.x, localMax.y, localMax.z),
        float3(localMax.x, localMax.y, localMax.z),
    };

    int outLeft = 0, outRight = 0, outBottom = 0, outTop = 0, outNear = 0, outFar = 0;

    [unroll]
    for (int i = 0; i < 8; i++)
    {
        float4 clip = mul(float4(corners[i], 1.0f), wvp);
        if (clip.x < -clip.w)
            outLeft++;
        if (clip.x > clip.w)
            outRight++;
        if (clip.y < -clip.w)
            outBottom++;
        if (clip.y > clip.w)
            outTop++;
        if (clip.z < 0)
            outNear++;
        if (clip.z > clip.w)
            outFar++;
    }

    return !(outLeft == 8 || outRight == 8 || outBottom == 8 || outTop == 8 || outNear == 8 || outFar == 8);
}

[numthreads(32, 1, 1)]
void main(uint gtid : SV_GroupIndex, uint3 gid : SV_GroupID)
{
    if (gtid == 0)
    {
        gVisibleCount = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    uint groupIndex = gid.x * 32 + gtid;

    StructuredBuffer<uint> faceCountPerGroup = ResourceDescriptorHeap[bakedFaceCountPerGroupSrvIndex];
    bool hasFaces = faceCountPerGroup[groupIndex] > 0;

    bool visible = false;
    if (hasFaces)
    {
        // CS/MSと同じ分解方法でこのグループのボクセル座標を復元
        uint3 groupDim = chunkDim / 2;
        uint3 groupCoord;
        groupCoord.x = groupIndex % groupDim.x;
        groupCoord.y = (groupIndex / groupDim.x) % groupDim.y;
        groupCoord.z = groupIndex / (groupDim.x * groupDim.y);
        int3 groupOrigin = int3(groupCoord) * 2;

        float3 localMin = chunkWorldOrigin + float3(groupOrigin) * blockSize;
        float3 localMax = localMin + float3(2, 2, 2) * blockSize; // 2x2x2ぶんの一辺

        visible = AABBInFrustum(localMin, localMax);
    }

    if (visible)
    {
        uint slot;
        InterlockedAdd(gVisibleCount, 1, slot);
        gPayload.groupIndices[slot] = groupIndex;
    }
    GroupMemoryBarrierWithGroupSync();

    DispatchMesh(gVisibleCount, 1, 1, gPayload);
}