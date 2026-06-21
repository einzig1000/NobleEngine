#pragma once
#include <Game.h>
#include <map>
#include <Utilities/PerlinNoise.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <GameObjects/MapManager/Chunk/Block/Block.h>

class BlockConfig;

struct InstanceData
{
	// ワールド行列
	Matrix4x4 World;
	// 色
	Vector4 Color;
	// ベーステクスチャID
	int32_t BaseTextureID = 0;
	// 破壊テクスチャID
	int32_t BreakTextureID = 0;
};

class Chunk
{
public:
	Chunk();
	~Chunk();
	void Update();
	void Draw(int32_t renderTargetID);

	// チャンクデータ生成
	void CreateChunkData(const NoiseParameter& param, const Vector3int& chunkIndex);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

	// 隣接チャンクの設定
	void SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor);
	bool IsNeighborExist(DirectionXYZ direction);

	// ブロック取得
	Block* GetBlock(const Vector3int& index);
	// AABB取得
	AABB GetAABB(const Vector3int& index);
	// 座標取得
	Vector3 LocalCenter(const Vector3int& index) const;

	// ブロック設置(置換)
	void SetBlock(const Vector3int& localIndex, const BlockID id);

	// blockPositionsの再構築
	void RebuildBlockPositions();


	// [localIndexのブロック]の露出状態を更新
	void RefreshExposeAt(const Vector3int& localIndex);
	// [localIndexのブロック]が露出状態を判定
	bool ComputeExposed(const Vector3int& localIndex);


	/// [チャンク内の全ブロック]     の露出状態を更新
	/// ↳ チャンク生成時
	void SetExposedAllBlocks();

	/// [localIndexの周り６ブロック] の露出状態を更新
	/// ↳ ブロック設置・破壊時
	void SetExposedAroundBlocks(const Vector3int& localIndex);

	/// [隣接チャンクの境界ブロック] の露出状態を更新
	/// ↳ 隣接チャンク生成時
	void SetExposedNeighborBlocks(const DirectionXYZ direction);
	
private:
	// 隣接チャンク
	std::unordered_map<DirectionXYZ, Chunk*> neighbors_;

	// Jsonから読み込まれていたか(初めての生成かどうか)
	// true : 既にマップのセーブデータに存在していたチャンク
	// false : 新規生成されたチャンク
	bool loadResult = false;

	// チャンク座標
	Vector3int chunkIndex_;
	// ブロックデータ配列
	Block blocks_[Constexprs::kChunkX][Constexprs::kChunkY][Constexprs::kChunkZ];
	// 描画オブジェクト
	std::unique_ptr<RenderObject> renderData_;
	// GPUインスタンスデータ配列
	std::vector<InstanceData> instanceDataList_;
	std::vector<Block*> instanceBlockMap_; // instanceDataList_ と同じ長さで、各スロットがどの Block* に対応するか
	std::vector<int> freeSlots_;           // 空きスロットのインデックス（再利用用）
	bool instanceBufferDirty_ = false;
	int32_t AllocateInstanceSlot(Block* b, const InstanceData& data);
	void FreeInstanceSlot(Block* b);

	std::unique_ptr<BlockConfig> blockConfig_;

	// ブロックごとの位置リスト Jsonから読み込んだデータを入れる箱　 いらなそう
	std::unordered_map<BlockID, std::vector<Vector3int>> blockPositions_;
};

