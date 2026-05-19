#pragma once
#include "Game.h"
#include <map>
#include "Utilities/PerlinNoise.h"

class Block;
class BlockConfig;

class Chunk
{
public:
	Chunk();
	~Chunk();
	void Update() const;
	void Draw() const;
	// インスタンスを作成する
	void CreateInstance();

	// チャンクデータ生成
	void CreateChunkData(const NoiseParameter& param, const Vector2int& chunkIndex);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param, const Vector2int& chunkIndex);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

	// 隣接チャンクの設定
	void SetNeighborChunk(DirectionXZ direction, Chunk* neighbor);
	bool IsNeighborExist(DirectionXZ direction);

	// ブロック取得  自チャンク+隣接チャンク対応
	Block* GetBlock(const Vector3int& index);

	// AABB取得  全チャンク対応
	AABB GetAABB(const Vector3int& index);

	// 座標取得  全チャンク対応
	Vector3 LocalCenter(const Vector3int& index) const;

	// ブロック設置(置換)  自チャンクのみ対応
	void SetBlock(const Vector3int& localIndex, const BlockID id) const;

	// ブロック破壊		   自チャンクのみ対応
	void DestroyBlock(const Vector3int& localIndex);

	// blockPositionsの再構築
	void RebuildBlockPositions();


	/// [localIndexのブロック]が露出状態を判定　自チャンクのみ対応
	bool ComputeExposed(const Vector3int& localIndex);

	/// [localIndexのブロック]の露出状態を更新　自チャンクのみ対応
	void RefreshExposeAt(const Vector3int& localIndex);


	/// [チャンク内の全ロック]       の露出状態を更新
	/// ↳ チャンク生成時
	void SetExposedAllBlocks();

	/// [localIndexの周り６ブロック] の露出状態を更新
	/// ↳ ブロック設置・破壊時
	void SetExposedAroundBlocks(const Vector3int& localIndex);

	/// [隣接チャンクの境界ブロック] の露出状態を更新
	/// ↳ 隣接チャンクが設定時
	void SetExposedNeighborBlocks(const DirectionXZ direction);

	// Jsonから読み込まれていたか(初めての生成かどうか)
	// true : 既にマップのセーブデータに存在していたチャンク
	// false : 新規生成されたチャンク
	bool loadResult = false;

	
	// チャンク座標
	Vector2int chunkIndex_;
	// ブロックごとの位置リスト
	//std::unordered_map<BlockID, std::vector<Vector3int>> blockPositions_;
	// ブロックデータ配列
	std::unique_ptr<Block> blocks_[CHUNK_X][CHUNK_Y][CHUNK_Z];
	// チャンクの全ブロックを描画するデータ
	std::unique_ptr<RenderData_Block> blockData_;

	std::unordered_map<DirectionXZ, Chunk*> neighbors_;

	std::unique_ptr<BlockConfig> blockConfig_;
};

