#pragma once
#include <Game.h>
#include <Utilities/PerlinNoise.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <GameObjects/Map/Chunk/Block/Block.h>

class BlockConfig;


struct ChunkInfo
{
	// 隣接チャンク１ブロック分も含めたブロックID配列SRVスロット
	//uint32_t blockIdSrvIndex = 0;
	// チャンクのボクセル数 (Constexprs::kChunkX/Y/Z、各軸偶数である必要あり)
	//Vector3int chunkDim;
	// このチャンクのワールド座標原点
	Vector3 chunkWorldOrigin;
	// ブロックサイズ (Constexprs::kBlockSize)
	float blockSize = Constexprs::kBlockSize;
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

	/// <summary>
	/// 指定座標のブロックを取得する
	/// </summary>
	/// <param name="index">thisから見たローカル座標</param>
	/// <param name="checkNeighborChunk">隣接チャンクまで探索しにいくか</param>
	/// <returns>ブロック</returns>
	Block* GetBlock(const Vector3int& index, bool checkNeighborChunk = false);


	// ブロック設置(置換)
	void SetBlock(const Vector3int& localIndex, const BlockID id);

private:

	// AABB取得
	AABB GetAABB(const Vector3int& index) const;
	// 座標取得
	Vector3 LocalCenter(const Vector3int& index) const;

	// 地形生成
	void CreateChunkData(const NoiseParameter& param);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

private:
	// 隣接チャンク
	std::unordered_map<DirectionXYZ, Chunk*> neighbors_;

	// チャンク座標
	Vector3int chunkIndex_;
	ChunkInfo chunkInfo_;
	// ブロックデータ配列
	Block blocks_[Constexprs::kChunkX][Constexprs::kChunkY][Constexprs::kChunkZ];
	std::vector<uint32_t> blockIds_;
	// 描画オブジェクト
	std::unique_ptr<RenderObject> render_;
	// 計算オブジェクト
	std::unique_ptr<ComputeObject> compute_;

	int32_t instanceBufferDirty_;

	// このチャンクのブロックID配列SRVスロット
	int32_t blockIdSrvIndex_ = -1;
	// 焼き込み済み面データSRVスロット
	int32_t bakedFaceBufferHandle_ = -1;
	// グループごとの面数SRVスロット
	int32_t faceCountBufferHandle_ = -1;
};

