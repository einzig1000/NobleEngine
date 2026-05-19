#pragma once
#include <deque>
#include "Game.h"
#include "Character/BaseCharacter.h"


class UIManager;
class Player;

class Enemy : public BaseCharacter
{
public:
	Enemy(Vector3 spawnPos);
	void SetPlayerPtr(Player* player) { playerptr_ = player; }
	~Enemy();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void UpdateViewRay();	// 視線レイ更新
	void UpdateMove();		// 移動更新


	// アイテムスロットにアイテムを追加
	void AddItemToItemslot(ItemID itemID);

	// 速度
	float speed_ = 0.15f;

private:
	UIMode currentMode_ = UIMode::None;

	Player* playerptr_;

	// 直近60フレームの自身の位置履歴
	std::deque<Vector3> recentPositions_;
};

