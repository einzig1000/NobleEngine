#pragma once
#include <GameManager/Phase/IPhase.h>
#include <TestParticle/TestParticle.h>
#include <TestAnimation/TestAnimation.h>

class Test2Phase : public IPhase
{
public:
	Test2Phase();
	~Test2Phase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }

private:
	TestParticle testParticle;
	TestAnimation testAnimation;

	std::unique_ptr<RenderObject> render_;
	std::unique_ptr<RenderObject> weapon_;
	Vector3 scale_ = Vector3{ 55.0f,55.0f,55.0f };
	Vector3 rotate_ = Vector3{ -1.310f, 0.0f, 0.0f };
	Vector3 translate_ = Vector3{ 0.0f, 0.0f, 0.0f };

	Vector3 cameraRotate_;

	float frame_ = 0.0f;

	int32_t rt_main_;
	int32_t rt_noise_;
	int32_t c_main_;

	int32_t t_dissolveMaskTexture_ = -1;
	float threshold = 0.2f;
};

