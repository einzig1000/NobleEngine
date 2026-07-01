#pragma once
#include <GameManager/Phase/IPhase.h>
#include <TestParticle/TestParticle.h>
#include <TestAnimation/TestAnimation.h>
#include <TestMeshShader/TestMeshShader.h>

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
	TestMeshShader testMeshShader;

	std::unique_ptr<RenderObject> render_;

	Vector3 cameraRotate_;

	float frame_ = 0.0f;

	int32_t rt_main_;
	int32_t c_main_;

	int32_t t_dissolveMaskTexture_ = -1;
	float threshold = 0.2f;
};

