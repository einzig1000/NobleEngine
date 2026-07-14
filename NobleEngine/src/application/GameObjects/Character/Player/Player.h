#pragma once
#include <Game.h>
#include <GameObjects/Character/BaseCharacter.h>

class Itemslot;
class UIManager;

class Player : public BaseCharacter
{
public:
	Player();
	~Player();

	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t renderTextureID) override;
	void DrawCrafting();
	void DrawInventory();
	void DrawHotbar();
	void DrawImGui() override;

	void UpdateViewRay(int32_t cameraID);	// 視線レイ更新
	void UpdateMove(int32_t cameraID);		// 移動更新
	void UpdateDash();		// ダッシュ更新
	void UpdateJump();		// ジャンプ更新

	// アイテムスロットにアイテムを追加
	void AddItemToItemslot(ItemID itemID);

	// 速度
	float speed_ = 0.15f;
	float normalSpeed_ = 0.13f;
	float dashSpeed_ = 0.20f;

	// ダッシュ関連
	int32_t wHeldFrames_ = 0;
	int32_t dashBufferTimer_ = 0;
	bool dash_ = false;


private:
	UIMode currentMode_ = UIMode::None;
};

