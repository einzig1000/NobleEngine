#pragma once
#include <memory>
#include <Game.h>
#include <GameObjects/Character/ICharacter.h>

class Itemslot;
class UIManager;
class SwingMining;
class RangeMining;

class Player : public ICharacter
{
public:
	Player();
	~Player();

	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t renderTextureID) override;
	void DrawImGui() override;


	
	void CheckExternalEvents();	// 外部イベント確認

	// 入力に対する更新
	void UpdateInput(int32_t cameraID);		
	// 左クリック時の処理(ブロック破壊とか攻撃とか)
	void UpdateInputLeftClick();
	// 右クリック時の処理(ブロック設置とか)
	void UpdateInputRightClick();
	// マウスカーソル操作時の処理(視線(カメラ)操作)
	void UpdateInputMouseCursor(int32_t cameraID);
	// WASD入力時の処理
	void UpdateInputWASD(int32_t cameraID);
	// SPACE入力時の処理
	void UpdateInputSpace();

	// 採掘モード切り替え
	void SetMiningPattern(MiningPattern pattern);
	MiningPattern GetMiningPattern() const { return miningMode_; }

	void SetViewCamera(int32_t cameraID);

	const Vector3& GetPosition() const { return translate_.value; }

private:
	// 速度
	float normalSpeed_ = 0.30f;
	float dashSpeed_ = 0.50f;

	// 視点カメラID
	int32_t c_viewCameraID_ = -1;

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

