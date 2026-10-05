#pragma once
#include <memory>
#include <Game.h>
#include <GameObjects/Character/ICharacter.h>
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>

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

	const MiningPointGauge* GetMiningPointGauge() const { return &miningPointGauge_; }

private:
	Matrix4x4 wvpMatrix_;

	// プレイヤーのテクスチャID
	int32_t t_player_ = -1;

	// 視点カメラID
	int32_t c_viewCameraID_ = -1;

	// 速度関連
	CounterSec dashBufferTimer_;
	bool dash_ = false;
	float normalSpeed_ = 5.0f;
	float dashSpeed_ = 7.0f;


	// 採掘ptの管理
	MiningPointGauge miningPointGauge_;


	// hp
	float previousHP_ = 0.0f;

	// 採掘
	std::unique_ptr<SwingMining> swingMining_;
	std::unique_ptr<RangeMining> rangeMining_;
	MiningPattern miningMode_ = MiningPattern::Swing;
};

