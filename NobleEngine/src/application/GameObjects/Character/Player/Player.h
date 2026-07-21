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
	void UpdateLeftClick();		// 左クリック時の処理(ブロック破壊とか攻撃とか)
	void UpdateRightClick();	// 右クリック時の処理(ブロック設置とか)
	void Draw(int32_t renderTextureID) override;
	void DrawImGui() override;

	void UpdateInput(int32_t cameraID);		// 入力更新
	void UpdateInputMove(int32_t cameraID);		// 移動更新
	void UpdateInputSpeed();						// ダッシュ更新
	void UpdateInputJump();						// ジャンプ更新

private:
	// 速度
	float normalSpeed_ = 0.30f;
	float dashSpeed_ = 0.50f;

	// ダッシュ関連
	int32_t dashBufferTimer_ = 0;
	bool dash_ = false;
};

