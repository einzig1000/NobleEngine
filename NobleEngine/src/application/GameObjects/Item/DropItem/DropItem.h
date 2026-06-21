#pragma once
#include "Game.h"
#include "Item/Item.h"

class Player;
class MapManager;

class DropItem
{
public:
	DropItem();
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetItem(ItemID id, Vector3 pos);
	void Update();
	void Draw();

	bool IsDestroy() const { return isDestroy_; }

private:

	std::unique_ptr<RenderData_Model> renderData_;

	// モデル・テクスチャデータ
	Item item_;
	// フレーム
	uint32_t frame_;

	float targetY_ = 0.0f;
	float velocityY_ = 0.0f;
	float accelerationY_ = GRAVITY;

	// 初期位置
	Vector3 InitPos_;
	// Y軸オフセット(sin揺れ用)
	float offsetYForSin_ = 0.0f;
	// Y軸オフセット(足元ブロックにめり込まないようにする用);
	float offsetYForGround_ = 0.0f;
	// 破棄フラグ
	bool isDestroy_;
	// 描画フラグ
	bool isDraw_;


	// 借り物
	Player* player_;
	MapManager* mapManager_;
};

