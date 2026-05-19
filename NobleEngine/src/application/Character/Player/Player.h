#pragma once
#include "Game.h"
#include "Character/BaseCharacter.h"

class Itemslot;
class UIManager;

class Player : public BaseCharacter
{
public:
	Player();
	~Player();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawCrafting();
	void DrawInventory();
	void DrawHotbar();
	void DrawImGui() override;

	void UpdateViewRay();	// 視線レイ更新
	void UpdateMove();		// 移動更新
	void UpdateDash();		// ダッシュ更新
	void UpdateJump();		// ジャンプ更新

	// アイテムスロットにアイテムを追加
	void AddItemToItemslot(ItemID itemID);

	// 速度
	float speed_ = 0.15f;
	float normalSpeed_ = 0.13f;
	float dashSpeed_ = 0.20f;

	// ダッシュ関連
	int wHeldFrames_ = 0;
	int dashBufferTimer_ = 0;
	bool dash_ = false;

	// レティクル
	RenderData_Sprite reticle_;

private:
	UIMode currentMode_ = UIMode::None;
};

