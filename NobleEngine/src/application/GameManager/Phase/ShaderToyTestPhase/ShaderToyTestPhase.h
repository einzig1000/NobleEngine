#pragma once
#include <GameManager/Phase/IPhase.h>
#include <memory>

class ShaderToyTestPhase : public IPhase
{
public:
	ShaderToyTestPhase();
	~ShaderToyTestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(Phase phase) override { nextPhase_ = phase; }

private:
	std::unique_ptr<RenderObject> render_;
	float seascapeTime_ = 0.0f;			// b0 : 経過秒数
	Vector2 seascapeResolution_{};		// b1 : 描画先RTのピクセルサイズ
	float seascapeSpeed_ = 1.0f;		// 時間の進む速さ(ImGuiで変更)
};