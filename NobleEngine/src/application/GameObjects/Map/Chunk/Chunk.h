#pragma once
#include <Game.h>
#include <Utilities/PerlinNoise.h>
#include <vector>

struct ChunkInfo
{
	// このチャンクのワールド座標原点
	Vector3 chunkWorldOrigin;
	// ブロックサイズ (Constexprs::kBlockSize)
	float blockSize = Constexprs::kBlockSize;
	// 
	Vector3uint chunkDim = Vector3uint(Constexprs::kChunkX, Constexprs::kChunkY, Constexprs::kChunkZ);
	// 
	uint32_t _pad0;
};

class Chunk
{
public:
	Chunk(const NoiseParameter& param, const Vector3int& chunkIndex);
	~Chunk();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);

	// 隣接チャンク管理
	void SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor);
	bool IsNeighborExist(DirectionXYZ direction);

	// 指定座標のブロックを取得する
	BlockID* GetBlockID(const Vector3int& index);
	BlockID* GetBlockIDHelloNeighbor(const Vector3int& index);

	// ブロック設置(置換)
	void SetBlock(const Vector3int& localIndex, const BlockID id);
	void SetBlockIDs(const Vector3int& localIndex, BlockID id);
	void PushToNeighborHalo(DirectionXYZ direction, const Vector3int& localIndex, const BlockID id);

private:

	// 地形生成
	void CreateChunkData(const NoiseParameter& param);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

private:
	// 隣接チャンク static_cast<size_t>(DirectionXYZ)でアクセス
	std::vector<Chunk*> neighbors_;

	// チャンク情報
	Vector3int chunkIndex_;
	ChunkInfo chunkInfo_;
	AABB chunkAABB_;
	// ブロックデータ配列
	BlockID blocks_[Constexprs::kChunkX][Constexprs::kChunkY][Constexprs::kChunkZ];
	std::vector<uint32_t> blockIds_;

	// 描画オブジェクト
	std::unique_ptr<RenderObject> render_;
	// 計算オブジェクト
	std::unique_ptr<ComputeObject> compute_;

	// ブロックID配列更新フラグ
	bool blockIdsDirty_ = false;
	void UpdateBlockIds();

	// このチャンクのブロックID配列SRVスロット
	int32_t blockIdSrvIndex_ = -1;
	// 焼き込み済み面データSRVスロット
	int32_t bakedFaceBufferHandle_ = -1;
	// グループごとの面数SRVスロット
	int32_t faceCountBufferHandle_ = -1;

	bool inCamera_ = false;
};

