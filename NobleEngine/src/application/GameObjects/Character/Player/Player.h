#pragma once
#include <memory>
#include <Game.h>
#include <GameObjects/Character/BaseCharacter.h>

class Itemslot;
class UIManager;
class SwingMining;
class RangeMining;

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

	bool JustDamaged() const { return previousHP_ > HP_; }	// ダメージを受けた瞬間

	// 採掘モード切り替え
	void SetMiningPattern(MiningPattern pattern);
	MiningPattern GetMiningPattern() const { return miningMode_; }

private:
	// 速度
	float normalSpeed_ = 0.30f;
	float dashSpeed_ = 0.50f;

	// ダッシュ関連
	int32_t dashBufferTimer_ = 0;
	bool dash_ = false;

	// hp
	float previousHP_ = 0.0f;

	// 採掘
	std::unique_ptr<SwingMining> swingMining_;
	std::unique_ptr<RangeMining> rangeMining_;
	MiningPattern miningMode_ = MiningPattern::Swing;
};

