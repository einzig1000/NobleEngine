#pragma once
#include "Game.h"
#include "definition/definition.h"
#include "Item/HaveItem/HaveItem.h"

class MapManager;
class UIManager;
class Block;

class BaseCharacter
{
public:
	virtual ~BaseCharacter() = default;

	virtual void Initialize() = 0;
	virtual void SetMapManager(MapManager* mapManager);
	virtual void SetUIManager(UIManager* uiManager);
	virtual void SetHaveItem() { haveItem_ = std::make_unique<HaveItem>(); }
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawImGui() = 0;

	virtual void TakeDamage(int32_t damage);

	// 移動
	virtual void Move(const Vector3& direction, float speed);
	// ジャンプ
	virtual void Jump();
	// ターゲットブロック破壊
	virtual void UpdateLeftClick();
	// ブロック設置
	virtual void SetNewBlock(BlockID id);


	// 見ているブロックをtargetBlock_にセットする
	virtual void SetTargetBlock();
	// 取得した視線レイをセットする
	virtual void SetViewRay(Ray ray) { viewRay_ = ray; }


	// アイテム獲得
	virtual void AddItem(ItemID id);

	// 持ってるアイテムによって攻撃力更新
	virtual void UpdateAttackPower();


	RenderData_Model data_;		// データ
	Ray viewRay_;				// 視線レイ
	float maxDistance = 6.0f;	// 視線範囲

protected:
	void UpdateGrounded(); // 接地判定更新

	std::unique_ptr<HaveItem> haveItem_; // 所持アイテム管理

	MapManager* mapManager_ = nullptr;
	RayHitResult target_;	// ターゲットにしているブロック
	RayHitResult preTarget_;	// 前フレームでターゲットにしていたブロック

	UIManager* uiManager_ = nullptr;
	

	float jumpPower_ = 0.1491f;	// ジャンプ力
	float breakPower_ = 1.0f;	// ブロック破壊力
	float attackPower_ = 1.0f; // 攻撃力

	int32_t HP_ = 20;			// 体力
	int32_t maxHP_ = 20;		// 最大体力
	int32_t defense_ = 0;		// 防御力


	bool isGrounded_ = false;
};

