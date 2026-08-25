#include "../3.0.0/Block.hlsli"

cbuffer SrvIndex : register(b0)
{
    // グループごとの露出面数のSRVスロット(CSが計算済みのものをそのまま使う)
    uint bakedFaceCountPerGroupSrvIndex;
};

struct Payload
{
    uint groupIndices[32];
};

groupshared Payload gPayload;
groupshared uint gVisibleCount;

[numthreads(32, 1, 1)]
void main(uint gtid : SV_GroupIndex, uint3 gid : SV_GroupID)
{
    if (gtid == 0)
    {
        gVisibleCount = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    // このASスレッドが担当する元のグループ番号
    uint groupIndex = gid.x * 32 + gtid;

    StructuredBuffer<uint> faceCountPerGroup = ResourceDescriptorHeap[bakedFaceCountPerGroupSrvIndex];
    bool visible = faceCountPerGroup[groupIndex] > 0;

    if (visible)
    {
        uint slot;
        InterlockedAdd(gVisibleCount, 1, slot);
        gPayload.groupIndices[slot] = groupIndex;
    }
    GroupMemoryBarrierWithGroupSync();

    // 生き残った数だけMSグループを起動する
    DispatchMesh(gVisibleCount, 1, 1, gPayload);
}