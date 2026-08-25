
struct FaceInfo
{
    int3 faceDir;
    uint4 faceCorners;
    float3 faceNormal;
};

struct BakedFace
{
    uint packedVoxelPos;
    uint faceIndex;
    uint blockId;
};

static const FaceInfo kFaceInfos[6] =
{
    { int3(1, 0, 0), uint4(1, 3, 7, 5), float3(1, 0, 0) },
    { int3(-1, 0, 0), uint4(0, 4, 6, 2), float3(-1, 0, 0) },
    { int3(0, 1, 0), uint4(2, 6, 7, 3), float3(0, 1, 0) },
    { int3(0, -1, 0), uint4(0, 1, 5, 4), float3(0, -1, 0) },
    { int3(0, 0, 1), uint4(5, 7, 6, 4), float3(0, 0, 1) },
    { int3(0, 0, -1), uint4(0, 2, 3, 1), float3(0, 0, -1) },
};

static const float3 kCubeCorners[8] =
{
    float3(0, 0, 0), float3(1, 0, 0), float3(0, 1, 0), float3(1, 1, 0),
    float3(0, 0, 1), float3(1, 0, 1), float3(0, 1, 1), float3(1, 1, 1),
};

static const uint kMaxFacesPerGroup = 48;
