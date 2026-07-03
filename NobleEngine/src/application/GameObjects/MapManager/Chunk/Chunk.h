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
};

class Chunk
{
public:
	Chunk();
	~Chunk();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);

	// 地形生成
	void CreateChunkData(const NoiseParameter& param, const Vector3int& chunkIndex);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

	// 隣接チャンク管理
	void SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor);
	bool IsNeighborExist(DirectionXYZ direction);

	// 露出状態計算
	void RefreshExposeAt(const Vector3int& localIndex);	// [localIndexのブロック]の露出状態を更新
	int32_t ComputeExposed(const Vector3int& localIndex);	// [localIndexのブロック]の露出状態を判定
	void SetExposedAllBlocks();									// [チャンク内の全ブロック]     の露出状態を更新
	void SetExposedAroundBlocks(const Vector3int& localIndex);	// [localIndexの周り６ブロック] の露出状態を更新
	void SetExposedNeighborBlocks(const DirectionXYZ direction);// [隣接チャンクの境界ブロック] の露出状態を更新

	// メッシュ生成
	void RefreshMeshData();	// メッシュデータを更新
	void Pushvertex(const Block* block);

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



	
private:
	// 隣接チャンク
	std::unordered_map<DirectionXYZ, Chunk*> neighbors_;

	// Jsonから読み込まれていたか(初めての生成かどうか)
	// true : 既にマップのセーブデータに存在していたチャンク
	// false : 新規生成されたチャンク
	bool loadResult = false;

	std::vector<VertexData> vertices_;
	Vector4int modelInfo;

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

