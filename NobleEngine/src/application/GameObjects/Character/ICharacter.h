#pragma once
#include <optional>
#include <Game.h>
#include <definition/definition.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/Character/HaveItem/HaveItem.h>

class MapManager;
class UIManager;
class Block;
class EventBus;

class ICharacter
{
private:

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
	virtual ~ICharacter() = default;

	virtual void Initialize() = 0;
	virtual void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	virtual void Update(int32_t cameraID) = 0;
	virtual void Draw(int32_t renderTextureID) = 0;
	virtual void UpdateHaveItem(int32_t cameraID);			// 手に持っているアイテムの更新
	virtual void DrawHaveItem(int32_t renderTextureID);		// 手に持っているアイテムを描画
	virtual void DrawImGui() = 0;

	virtual void SetID(int32_t id) { characterID_ = id; }
	virtual int32_t GetID() const { return characterID_; }

	virtual void TakeDamage(int32_t damage);

	virtual void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }

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
	virtual void DestroyBlockInOBB(const OBB& obb);
	virtual void DestroyBlockInSphere(const Sphere& sphere);
	// マップに自身を登録
	virtual void RegisterToMap();

	// 見ているブロックをtargetBlock_にセットする
	virtual void SetTargetBlock();

	virtual void ComputeViewRay(int32_t cameraID);

	// 視線上にあるブロックを取得。target_/SetTargetBlockはGetFirstHitByRayが未実装のため常にNoneになる点に注意
	virtual std::optional<lookAtBlock> GetLookedAtBlock() const;
	// 指定ブロックのワールドAABBを取得
	virtual AABB GetBlockWorldAABB(const Vector3int& chunkIndex, const Vector3int& localIndex) const;


	// インベントリの読み取り専用参照
	virtual const ItemInventory* GetInventory() const { return &inventory_; }
	// アイテム獲得
	virtual void AddItem(ItemID id) { inventory_.AddItem(id); }
	// 手に持っているアイテムを取得
	virtual const ItemID GetHaveItem() const { return inventory_.GetCurrentSelectedItemID(); }

	// 手に持っているアイテムのコライダーを取得
	virtual const ColliderShape& GetHaveItemWorldCollider() const { return haveItem_.GetWorldCollider(); }


protected:
	EventBus* eventBus_ = nullptr;

	RenderObject render_;		// 描画データ

	int32_t characterID_ = -1;		// キャラクターID

	Matrix4x4 worldMatrix_;		// 自身のワールド行列

	float jumpPower_ = 0.1f;	// ジャンプ力
	float speed_ = 0.10f;		// 移動速度
	int32_t HP_ = 20;			// 体力
	int32_t maxHP_ = 20;		// 最大体力
	int32_t defense_ = 0;		// 防御力

	VectorDynamics translate_;
	VectorDynamics scale_;
	VectorDynamics rotate_;

	float viewPhi_ = 0.0f;
	float viewTheta_ = 0.0f;	// std::numbers::pi_v<float> / 2.0f;
	float lookSensitivity_ = 0.003f;			// マウス感度（ラジアン/ピクセル）
};

