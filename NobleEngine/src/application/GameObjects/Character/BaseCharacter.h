#pragma once
#include <Game.h>
#include <definition/definition.h>
//#include <Item/HaveItem/HaveItem.h>
#include <DrawSystem/RenderData/RenderObject.h>

class MapManager;
class UIManager;
class Block;

class BaseCharacter
{
public:
	virtual ~BaseCharacter() = default;

	virtual void Initialize() = 0;
	virtual void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	virtual void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; }
	//virtual void SetHaveItem() { haveItem_ = std::make_unique<HaveItem>(); }
	virtual void Update(int32_t cameraID) = 0;
	virtual void Draw(int32_t renderTextureID) = 0;
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

	// カメラID取得
	virtual int32_t GetCameraID() const { return c_viewCameraID_; }
	// 見ているブロックをtargetBlock_にセットする
	virtual void SetTargetBlock();
	// 取得した視線レイをセットする
	virtual void SetViewRay(Ray ray) { viewRay_ = ray; }


	// アイテム獲得
	virtual void AddItem(ItemID id);

	// 持ってるアイテムによって攻撃力更新
	virtual void UpdateAttackPower();

	RenderObject render_;		// データ
	Ray viewRay_;				// 視線レイ
	float maxDistance = 6.0f;	// 視線範囲

protected:
	UIManager* uiManager_ = nullptr;
	MapManager* mapManager_ = nullptr;

	//std::unique_ptr<HaveItem> haveItem_; // 所持アイテム管理

	VectorDynamics translate_;
	VectorDynamics scale_;
	VectorDynamics rotate_;


	int32_t c_viewCameraID_ = -1;	// 視線カメラID
	RayHitResult target_;	// ターゲットにしているブロック
	RayHitResult preTarget_;	// 前フレームでターゲットにしていたブロック

	float jumpPower_ = 0.3f;	// ジャンプ力
	float breakPower_ = 1.0f;	// ブロック破壊力
	float attackPower_ = 1.0f; // 攻撃力
	AABB aabb_;	// 当たり判定

	int32_t HP_ = 20;			// 体力
	int32_t maxHP_ = 20;		// 最大体力
	int32_t defense_ = 0;		// 防御力

	bool isGrounded_ = false;
};

