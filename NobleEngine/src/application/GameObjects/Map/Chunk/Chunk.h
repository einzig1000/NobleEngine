#pragma once
#include <Game.h>
#include <PerlinNoise.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <GameObjects/Map/Chunk/Block/Block.h>

class BlockConfig;

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
	void RefreshExposeAt(const Vector3int& localIndex);			// [localIndexのブロック]の露出状態を更新
	int32_t ComputeExposed(const Vector3int& localIndex);		// [localIndexのブロック]の露出状態を判定
	void SetExposedAllBlocks();									// [チャンク内の全ブロック]     の露出状態を更新
	void SetExposedAroundBlocks(const Vector3int& localIndex);	// [localIndexの周り６ブロック] の露出状態を更新
	void SetExposedNeighborBlocks(const DirectionXYZ direction);// [隣接チャンクの境界ブロック] の露出状態を更新

	// メッシュ生成
	void RefreshMeshData();	// メッシュデータを更新
	void Pushvertex(const Block* block);
	void PushvertexOptimized(const Block* block);

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

	// 頂点配列(頂点を限界まで減らした最適化配列)
	std::vector<VertexData> optimizedVertices_;
	// 頂点配列(トライアングルリスト)
	std::vector<VertexData> vertices_;
	// color配列(vertices_と対応)
	std::vector<uint32_t> vertexColors_;
	// color配列ヒープインデックス
	int32_t colorHeapIndex_ = -1;
	Vector4int modelInfoHeapIndex;

	// チャンク座標
	Vector3int chunkIndex_;
	// ブロックデータ配列
	Block blocks_[Constexprs::kChunkX][Constexprs::kChunkY][Constexprs::kChunkZ];
	// 描画オブジェクト
	std::unique_ptr<RenderObject> renderData_;
	bool instanceBufferDirty_ = false;

	static const BlockConfig blockConfig_;
};

