#pragma once
#include <GameManager/Phase/PhaseParent/PhaseParent.h>
#include <TestAnimation/TestAnimation.h>
#include <TestParticle/TestParticle.h>
#include <memory>

class TestPhase :
	public PhaseParent
{
public:
	TestPhase();
	~TestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }



private:
	std::unique_ptr<RenderObject> cbvOnly_;
	EulerTransforms transform1_;
	Vector4 color1_;
	std::unique_ptr<RenderObject> cbvAndSrv_;
	EulerTransforms transform2_[10];
	Vector4 color2_[10];
	int32_t tex2_[10];
	std::unique_ptr<RenderObject> line_;
	std::unique_ptr<RenderObject> skybox_;
	std::unique_ptr<RenderObject> PunctualLight_;
	LightDataForGPU lightData_;
	Material materialData_;
	std::unique_ptr<RenderObject> environmentMap_;

	std::unique_ptr<RenderObject> postEffect1_;
	std::unique_ptr<RenderObject> postEffect2_;

	std::unique_ptr<RenderObject> screenDrawObjectMain_;
	EulerTransforms mainScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMiniMap_;
	EulerTransforms miniMapScreenTransform_;


	std::unique_ptr<TestAnimation> testAnimation_;
	std::unique_ptr<TestParticle> testParticle_;

	int32_t rt_main_;
	int32_t rt_miniMap_;
	int32_t rt_Vignette_;
	int32_t rt_GrayScale_;
	int32_t t_uvChecker;
	int32_t t_monsterBall_;
	int32_t t_white1x1_;
	int32_t t_dds_;

	uint32_t audio1;
	uint32_t audio2;
};