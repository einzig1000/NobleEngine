#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/Character/HaveItem/HaveItem.h>

class MapManager;
class UIManager;
class Block;

class BaseCharacter
{
private:

	UIManager* uiManager_ = nullptr;
	MapManager* mapManager_ = nullptr;

	ItemInventory inventory_;	// 所持アイテム管理
	HaveItem haveItem_;	// 手に持ってるアイテムの描画

	Ray viewRay_;				// 視線レイ
	float maxDistance = 6.0f;	// 視線範囲
	RayHitResult target_;		// ターゲットにしているブロック
	RayHitResult preTarget_;	// 前フレームでターゲットにしていたブロック

	AABB aabb_;					// 当たり判定
	bool isGrounded_ = false;	// 接地判定

public:
	virtual ~BaseCharacter() = default;

	virtual void Initialize() = 0;
	virtual void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	virtual void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; }
	virtual void Update(int32_t cameraID) = 0;
	virtual void Draw(int32_t renderTextureID) = 0;
	virtual void UpdateHaveItem(int32_t cameraID);			// 手に持っているアイテムの更新
	virtual void DrawHaveItem(int32_t renderTextureID);		// 手に持っているアイテムを描画
	virtual void DrawImGui() = 0;

	virtual void TakeDamage(int32_t damage);


	// 移動
	virtual void Move(const Vector3& direction, float speed);
	// ジャンプ
	virtual void Jump();
	// マップ情報を見て適切に移動を適用する
	virtual void ApplyMove();
	// 衝突判定用AABBをセット
	virtual void SetBoundingBox(const AABB& aabb) { aabb_ = aabb; }



	// ブロック設置
	virtual void SetBlock(BlockID id);
	// ブロック範囲破壊
	virtual void DestroyBlockInAABB(const AABB& aabb);
	// マップに自身を登録
	virtual void RegisterToMap();

	// カメラID取得
	virtual int32_t GetCameraID() const { return c_viewCameraID_; }
	// 見ているブロックをtargetBlock_にセットする
	virtual void SetTargetBlock();

	virtual void ComputeViewRay();


	// アイテム獲得
	virtual void AddItem(ItemID id) { inventory_.AddItem(id); }
	// 手に持っているアイテムを取得
	virtual ItemID GetHaveItem() const { return inventory_.GetCurrentSelectedItemID(); }
	// 手に持っているアイテムのAABBを取得
	virtual std::vector<AABB> GetHaveItemAABB() const { return haveItem_.GetAABB(); }


protected:

	RenderObject render_;		// データ

	Matrix4x4 worldMatrix_;		// 自身のワールド行列

	int32_t c_viewCameraID_ = -1;	// 視線カメラID

	float jumpPower_ = 0.3f;	// ジャンプ力
	float speed_ = 0.30f;		// 移動速度
	int32_t HP_ = 20;			// 体力
	int32_t maxHP_ = 20;		// 最大体力
	int32_t defense_ = 0;		// 防御力

	VectorDynamics translate_;
	VectorDynamics scale_;
	VectorDynamics rotate_;
};

