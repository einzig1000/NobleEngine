#pragma once
#include <GameManager/Phase/IPhase.h>
#include <TestParticle/TestParticle.h>


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
	TestParticle TestParticle;

	Vector3 cameraRotate_;

	float frame_ = 0.0f;

	int32_t rt_main_;
	int32_t c_main_;
};

